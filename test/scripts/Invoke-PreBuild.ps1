<#
#̷𝓍   𝓐𝓡𝓢 𝓢𝓒𝓡𝓘𝓟𝓣𝓤𝓜
#̷𝓍   🇵​​​​​🇴​​​​​🇼​​​​​🇪​​​​​🇷​​​​​🇸​​​​​🇭​​​​​🇪​​​​​🇱​​​​​🇱​​​​​ 🇸​​​​​🇨​​​​​🇷​​​​​🇮​​​​​🇵​​​​​🇹​​​​​ 🇧​​​​​🇾​​​​​ 🇬​​​​​🇺​​​​​🇮​​​​​🇱​​​​​🇱​​​​​🇦​​​​​🇺​​​​​🇲​​​​​🇪​​​​​🇵​​​​​🇱​​​​​🇦​​​​​🇳​​​​​🇹​​​​​🇪​​​​​.🇶​​​​​🇨​​​​​@🇬​​​​​🇲​​​​​🇦​​​​​🇮​​​​​🇱​​​​​.🇨​​​​​🇴​​​​​🇲​​​​​
#>


function Invoke-IsAdministrator  {  
    (New-Object Security.Principal.WindowsPrincipal ([Security.Principal.WindowsIdentity]::GetCurrent())).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)  
}
function Get-ScriptDirectory {
    Split-Path -Parent $PSCommandPath
}

      try{

        [string]$ErrorDetails=''
        [System.Boolean]$ErrorOccured=$False

        ########################################################################################
        #
        #  Read Arguments 
        #
        ########################################################################################
        $ExpectedNumberArguments = 2
        $Script:Arguments = $args.Clone()
        [System.Collections.ArrayList]$ArgumentList = [System.Collections.ArrayList]::new()
        0..$ExpectedNumberArguments |  % {
            $index = $_
            if([string]::IsNullOrEmpty($Script:Arguments[$index]) -eq $True){
             Write-Output "missing argument $index"
            }
            [void]$ArgumentList.Add("$($Script:Arguments[$index])")
        }

        [Bool]$DebugMode                = ([string]::IsNullOrEmpty($Script:Arguments[3]) -eq $False)

        [string]$TargetName             = $ArgumentList.Item(0)
        [string]$SolutionDirectory      = $ArgumentList.Item(1)
        [string]$ResCryptBinary         = $ArgumentList.Item(2)

        [string]$SolutionDirectory      = (Resolve-Path "$SolutionDirectory").Path
        [string]$SourcePath             = (Resolve-Path "$SolutionDirectory\src").Path
        [string]$ScriptsDirectory       = (Resolve-Path "$SolutionDirectory\scripts").Path
        [string]$ResCryptBinary         = (Resolve-Path "$ResCryptBinary").Path
        [string]$StringsCxr             = "$SourcePath\strings.cxr"
        [string]$StringsCpp             = "$SourcePath\strings.cpp"
        [string]$TmpFile                = "$ENV:TEMP\Out.txt"
        [string]$GitExe                 = (Get-Command 'git.exe').Source

        Write-Debug "########################################################################################"
        Write-Debug "                                    DEBUG ARGUMENTS                                     "
        Write-Debug "`tTargetName          ==> $TargetName"
        Write-Debug "`tSolutionDirectory   ==> $SolutionDirectory"
        Write-Debug "########################################################################################"

        ########################################################################################
        #
        #  Get Dependencies
        #
        ########################################################################################

        [string[]]$deps = . "$ScriptsDirectory\dependencies\GetDependencies.ps1" -Path "$ScriptsDirectory\dependencies"
        $depscount = $deps.Count
        $deps | % {
            $file = $_
            . "$file"
            Write-Debug "Include `"$file`""
        }
       
        $Test = Test-Dependencies -Quiet
        if(! ($Test) ) { throw "dependencies error"} 

        [System.Boolean]$IsAdministrator = Invoke-IsAdministrator 

        <#
        Remove-Item "$StringsCpp" -Force -ErrorAction Ignore
        Write-Output "====================================================="
        Write-Output " GENERATING ENCRYPTED STRINGS in strings.cpp"
        Write-Output "`"$ResCryptBinary`" `"-i`" `"$StringsCxr`" `"-o`" `"$StringsCpp`""
        Write-Output "====================================================="
        &"$ResCryptBinary" "-i" "$StringsCxr" "-o" "$StringsCpp" *> "$TmpFile"
        [string[]]$Out = Get-Content "$TmpFile"
        $Success = $Out[$Out.Count -1].Contains("created")
        if($False -eq $Success) { throw "FAILED TO ENCYPT STRINGS $Out"}
        #>
        
        $InputRcFile = "$SourcePath\EmbedResources.rc"
        $OutputRcFile = "$SourcePath\EncryptedResources.rc"
        $Password="TooManySecrets_"
        Write-BuildOutTitle "ENCRYPT RESOURCES"

        Invoke-EncryptRcFileString -InputRcFile "$InputRcFile" -OutputRcFile "$OutputRcFile" -Password "$Password" -SolutionDirectory "$SolutionDirectory"
        #Write-Output "Invoke-EncryptRcFileString -InputRcFile `"$InputRcFile`" -OutputRcFile `"$OutputRcFile`" -Password `"$Password`" -SolutionDirectory `"$SolutionDirectory`""
        Write-Output " `"$InputRcFile`" ==> `"$OutputRcFile`"" 
    }catch{
        Write-Error "$_"
    }
