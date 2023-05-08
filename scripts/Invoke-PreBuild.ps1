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
        [string]$CXRBinary               = $ArgumentList.Item(2)

        [string]$SolutionDirectory      = (Resolve-Path "$SolutionDirectory").Path
         [string]$TestDirectory        = (Resolve-Path "$SolutionDirectory\Test").Path
        [string]$CXRBinary               = (Resolve-Path "$CXRBinary").Path
        [string]$StringsCxr             = "$TestDirectory\strings.cxr"
        [string]$StringsCpp             = "$TestDirectory\strings.cpp"
        [string]$TmpFile                = "$ENV:TEMP\Out.txt"
        Write-Debug "########################################################################################"
        Write-Debug "                                    DEBUG ARGUMENTS                                     "
        Write-Debug "`tTargetName          ==> $TargetName"
        Write-Debug "`tPlBinary            ==> $CXRBinary"
        Write-Debug "`tSolutionDirectory   ==> $SolutionDirectory"
        Write-Debug "########################################################################################"


        Remove-Item "$StringsCpp" -Force -ErrorAction Ignore
        Write-Output "====================================================="
        Write-Output " GENERATING ENCRYPTED STRINGS in strings.cpp"
        Write-Output "`"$CxrBinary`" `"-i`" `"$StringsCxr`" `"-o`" `"$StringsCpp`""
        Write-Output "====================================================="
        &"$CxrBinary" "-i" "$StringsCxr" "-o" "$StringsCpp" *> "$TmpFile"
        [string[]]$Out = Get-Content "$TmpFile"
        $Success = $Out[$Out.Count -1].Contains("created")
        if($False -eq $Success) { throw "FAILED TO ENCYPT STRINGS $Out"}
    }catch{
        Write-Error "$_"
    }
