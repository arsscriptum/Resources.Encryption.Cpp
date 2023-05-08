
<#
#̷𝓍   𝓐𝓡𝓢 𝓢𝓒𝓡𝓘𝓟𝓣𝓤𝓜 
#̷𝓍   
#̷𝓍   REDDIT SUPPORT CODE SAMPLE
#̷𝓍   Guillaume Plante <guillaume.qc@gmail.com>   
#>

[CmdletBinding(SupportsShouldProcess)]
param(
    [Parameter(Mandatory=$false)]
    [switch]$ShowDebug
)


function Get-ResourceFilePath{
    [CmdletBinding(SupportsShouldProcess)]
    param()
    try{
        $SourceRoot =   (Resolve-Path "$Script:SolutionDirectory\src").Path
     
        $ResoureFilePath = (Get-ChildItem -Path $SolutionRoot -File -Filter "*.rc" -Recurse).Fullname

        Write-Verbose "ResoureFilePath $$ResoureFilePath"
        return $ResoureFilePath
    }catch{
        Write-Error $_
    }
}


function Get-EncryptedString{
    [CmdletBinding(SupportsShouldProcess)]
    param(
        [Parameter(Mandatory=$true,  HelpMessage="Input String", Position=0)]
        [string]$String,
        [Parameter(Mandatory=$true,  HelpMessage="Input Password", Position=1)]
        [string]$Password
    )
    try{

        $ResCryptExe =   (Resolve-Path "$Script:SolutionDirectory\rescrypt.exe").Path
        

        Write-Verbose "`"$ResCryptExe`" `"-s`" `"$String`" `"-p`" `"$Password`" `"-q`" "
        &"$ResCryptExe" "-s" "$String" "-p" "$Password" "-q" *> "$ENV:Temp\Results.out"
        $Results = Get-Content "$ENV:Temp\Results.out"
        $ResultCount = $Results.Count 

        if($ResultCount -ne 2){ throw "Invalid Result"}
        if($Results[0] -ne "SUCCESS"){ throw "Invalid Result"}
        $EncryptedString = $Results[1]
        Write-Verbose "EncryptedString $EncryptedString"
        return $EncryptedString
    }catch{
        Write-Error $_
    }
}


function Invoke-EncryptRcFileString{
    [CmdletBinding(SupportsShouldProcess)]
    param(
        [Parameter(Mandatory=$true,  HelpMessage="Input file", Position=0)]
        [string]$InputRcFile,
        [Parameter(Mandatory=$true,  HelpMessage="Input file", Position=1)]
        [string]$OutputRcFile,
        [Parameter(Mandatory=$true,  HelpMessage="Input Password", Position=2)]
        [string]$Password,
        [Parameter(Mandatory=$true,  HelpMessage="Input Password", Position=3)]
        [string]$SolutionDirectory,
        [Parameter(Mandatory=$false)]
        [switch]$ShowDebug
    )
    try{ 
        $Script:SolutionDirectory = $SolutionDirectory
        $Content = Get-Content -Path $InputRcFile
        [System.Collections.ArrayList]$StringTableContent = [System.Collections.ArrayList]::new()
        $index = 0
        $StringTableIndexValue = @{}
        $StringTableIndexValueEncrypted = @{}
        $StringTableFound = $False
        $ContentLength = $Content.Length
        [regex]$StringTablePattern = [regex]::new('(?<StringId>IDR_STRING[0-999])(?<Separation>[\s]+)(?<StringContent>[\x20-x7Fa-zA-Z0-9]*)')
        Write-Verbose "Path $InputRcFile"
        Write-Verbose "ContentLength $ContentLength"  
        for($i = 0 ; $i -lt $ContentLength ; $i++){
            $index = $i
            $line = $Content[$index]
            Write-Verbose "line $line"
            if($line.Contains('STRINGTABLE')){
                $StringTableFound = $True
                
                if($line.Contains('{')){
                    break;    
                }
                while($line.Contains('{') -eq $False){
                    $index++
                    $line = $Content[$index]
                }

                break;
                
            }
        }

        if($False -eq $StringTableFound){
            Write-Error "Cannot find STRING TABLE"
        }

        while($line.Contains('}') -eq $False){
            $index++
            $line = $Content[$index]
            if($line -match $StringTablePattern){
                [void]$StringTableContent.Add($line)
                $StringTableIndexValue.Add($index, "$line")
            }
        }
        
        if($ShowDebug){
            Write-Verbose "String Table Content"
            Write-Verbose "====================" 
            $StringTableContent | % {
                Write-Verbose "$_"
            }
        }

        $ValidIndexes = New-Object Collections.Generic.List[Int]
        $StringTableIndexValue.keys | ForEach-Object {
            $LineIndex = $_
            $ClearString = "$($StringTableIndexValue[$_])"
            if($ClearString -match $StringTablePattern){
                $StringId = $Matches.StringId
                $StringCnt = $Matches.StringContent
                $ValidIndexes.Add($LineIndex)
                $StringCnt = $StringCnt.TrimStart("`"").TrimEnd("`"")
                $EncryptedString = Get-EncryptedString $StringCnt "$Password"
  
                $StartOfString = $ClearString.IndexOf($StringCnt)
                $NewLine = $ClearString.SubString(0, $StartOfString-1)
                $NewLine = $NewLine + "`"$EncryptedString`""
                
                $StringTableIndexValueEncrypted.Add($LineIndex, $NewLine)
            }    
        }


        [System.Collections.ArrayList]$NewFileContent = [System.Collections.ArrayList]::new()
        for($i = 0 ; $i -lt $ContentLength ; $i++){
            $index = $i
            $line = $Content[$index]
            if($ValidIndexes.Contains($index)){
                $NewEncryptedLine = $StringTableIndexValueEncrypted[$index]
                [void]$NewFileContent.Add($NewEncryptedLine)
            }else{
                [void]$NewFileContent.Add($line)
            }
        }


        if($ShowDebug){
            Write-Host "Original" -f DarkRed 
            Write-Host "==================================" -f DarkYellow 
            $Content | % {
                Write-Host "$_" -f DarkYellow
            }
            Write-Host "Encrypted" -f DarkRed 
            Write-Host "==================================" -f DarkYellow 
            $NewFileContent | % {
                Write-Host "$_" -f DarkYellow
            }
        }

        $FileInfo = Get-Item "$Path"

        $dir = $FileInfo.DirectoryName
        $base = $FileInfo.Basename
        $ext = $FileInfo.Extension
        #$BackupPath = "{0}\{1}-BACKUP{2}" -f $dir, $base, $ext

        #Write-Verbose "Write backup file $BackupPath"
        #Copy-Item $Path $BackupPath -Force -ErrorAction Ignore
        Set-Content -Path $OutputRcFile -Value $NewFileContent
    }catch{
        Write-Error $_
    }
}


<#

$InputRcFile = "F:\Development2\Cpp.Resources.Embed\src\EmbedResources.rc"
$OutputRcFile = "F:\Development2\Cpp.Resources.Embed\src\EncryptedResources.rc"
$Password="TooManySecrets_"

Invoke-EncryptRcFileStrings -InputRcFile $InputRcFile -OutputRcFile $OutputRcFile -Password $Password -ShowDebug:$ShowDebug


#>