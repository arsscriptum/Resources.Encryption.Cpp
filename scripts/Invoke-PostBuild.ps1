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
        $ExpectedNumberArguments = 4
        $Script:Arguments = $args.Clone()
        [System.Collections.ArrayList]$ArgumentList = [System.Collections.ArrayList]::new()
        0..$ExpectedNumberArguments |  % {
            $index = $_
            if([string]::IsNullOrEmpty($Script:Arguments[$index]) -eq $True){
             Write-Output "missing argument $index"
            }
            [void]$ArgumentList.Add("$($Script:Arguments[$index])")
        }

        $CopyDejaInsightLibraries       = $True
        [string]$Configuration          = $ArgumentList.Item(0)
        [string]$Platform               = $ArgumentList.Item(1)
        [string]$SolutionDirectory      = $ArgumentList.Item(2)
        [string]$TestDirectory          = $ArgumentList.Item(3)
        [string]$BuiltBinary            = $ArgumentList.Item(4)

        [string]$SolutionDirectory      = (Resolve-Path "$SolutionDirectory").Path
        [string]$BuiltBinary            = (Resolve-Path "$BuiltBinary").Path

        [string]$OutputDirectory        = (Resolve-Path "$SolutionDirectory\bin\$Platform\$Configuration").Path
        [string]$ScriptsDirectory       = Join-Path $SolutionDirectory 'scripts'
        [string]$ProjectsPath           = Join-Path $SolutionDirectory 'vs'
        [string]$MD5HASH                = (Get-FileHash -Path "$BuiltBinary" -Algorithm MD5).Hash

        Write-Debug "########################################################################################"
        Write-Debug "                                    DEBUG ARGUMENTS                                     "
        Write-Debug "`tPlatform            ==> $Platform"
        Write-Debug "`tConfiguration       ==> $Configuration"
        Write-Debug "`tSolutionDirectory   ==> $SolutionDirectory"
        Write-Debug "`tScriptsDirectory    ==> $ScriptsDirectory"
        Write-Debug "`tOutputDirectory     ==> $OutputDirectory"
        Write-Debug "`tBuiltBinary         ==> $BuiltBinary"
        Write-Debug "`tTestDirectory       ==> $TestDirectory"
        Write-Debug "########################################################################################"

        if(!(Test-Path "$BuiltBinary")){
            throw "ERROR: NO BINARY FOUND @`"$BuiltBinary`""
        }

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

       

        ########################################################################################
        #
        #  Copy DejaInsight Libraries
        #
        ########################################################################################


        if ($CopyDejaInsightLibraries -eq $True) {
            Write-BuildOutTitle "COPY DEJAINSIGHT LIBRARY ($Platform)"
            if($Platform -eq 'x64'){
                $DejaInsighDll = "$ENV:DejaToolsRootDirectory\lib\DejaInsight.x64.dll"
                Copy-Item $DejaInsighDll $OutputDirectory -Force
                Write-Output "[DEJA $Platform Dll] `"$DejaInsighDll`" ==> `"$OutputDirectory`""
            }else{
                $DejaInsighDll = "$ENV:DejaToolsRootDirectory\lib\DejaInsight.x86.dll"
                Copy-Item $DejaInsighDll $OutputDirectory -Force
                Write-Output "[DEJA $Platform Dll] `"$DejaInsighDll`" ==> `"$OutputDirectory`""
            }
        }


        Write-BuildOutTitle "COPY BINARY TO TEST ($Platform)"
        Write-Output "$BuiltBinary => $TestDirectory"
        Copy-Item "$BuiltBinary" "$TestDirectory"
    }catch{
        #Enable-ExceptionDetailsTextBox
        ##Disable-ExceptionDetailsTextBox
        #Show-ExceptionDetails $_ -ShowStack
        [System.Management.Automation.ErrorRecord]$Record=$_
        $Line = [string]::new('=',120)
        $Spaces = [string]::new(' ',49)
        $ErrorOccured=$True
        Write-Output "`n`n`n`n$Line`n$Spaces POST BUILD SCRIPT ERROR`n$Line"
        $formatstring = "{0} {1}"
        $fields = $Record.FullyQualifiedErrorId,$Record.Exception.ToString()
        $ExceptMsg=($formatstring -f $fields)
        $Stack=$Record.ScriptStackTrace
        Write-Output "$ExceptMsg"
        Write-Output "$Stack" 
        
        exit -9
    }   