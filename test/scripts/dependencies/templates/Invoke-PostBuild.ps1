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
        $ExpectedNumberArguments = 3
        $Script:Arguments = $args.Clone()
        [System.Collections.ArrayList]$ArgumentList = [System.Collections.ArrayList]::new()
        0..$ExpectedNumberArguments |  % {
            $index = $_
            if([string]::IsNullOrEmpty($Script:Arguments[$index]) -eq $True){
             Write-Output "missing argument $index"
            }
            [void]$ArgumentList.Add("$($Script:Arguments[$index])")
        }

        [string]$Configuration          = $ArgumentList.Item(0)
        [string]$Platform               = $ArgumentList.Item(1)
        [string]$SolutionDirectory      = $ArgumentList.Item(2)
        [string]$BuiltBinary            = $ArgumentList.Item(3)

        [string]$SolutionDirectory      = (Resolve-Path "$SolutionDirectory").Path
        [string]$BuiltBinary            = (Resolve-Path "$BuiltBinary").Path

        [string]$OutputDirectory        = (Resolve-Path "$SolutionDirectory\bin\$Platform\$Configuration").Path
        [string]$ScriptsDirectory       = Join-Path $SolutionDirectory 'scripts'
        [string]$ProjectsPath           = Join-Path $SolutionDirectory 'vs'
        [string]$ReadmeFile             = Join-Path "$SolutionDirectory" "usage.txt"
        [string]$CfgIniFile             = Join-Path "$SolutionDirectory" "config.ini"
        [string]$VersionFile            = Join-Path "$ProjectsPath" "version.nfo"
        [string]$StatsFile              = Join-Path "$ProjectsPath" "stats.json"
        [string]$TmpStatsFile           = Join-Path "$ENV:TEMP" "stats_server.json"
        [string]$MD5HASH                = (Get-FileHash -Path "$BuiltBinary" -Algorithm MD5).Hash

        Write-Debug "########################################################################################"
        Write-Debug "                                    DEBUG ARGUMENTS                                     "
        Write-Debug "`tPlatform            ==> $Platform"
        Write-Debug "`tConfiguration       ==> $Configuration"
        Write-Debug "`tSolutionDirectory   ==> $SolutionDirectory"
        Write-Debug "`tScriptsDirectory    ==> $ScriptsDirectory"
        Write-Debug "`tOutputDirectory     ==> $OutputDirectory"
        Write-Debug "`tBuiltBinary         ==> $BuiltBinary"
        Write-Debug "`tReadmeFile          ==> $ReadmeFile"
        Write-Debug "`tCfgIniFile          ==> $CfgIniFile"
        Write-Debug "`tVersionFile         ==> $VersionFile"
        Write-Debug "`tStatsFile           ==> $StatsFile"
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
        #  READ INI FILE
        #
        ########################################################################################
        [System.Boolean]$ConfigureServiceRegistration = $False
        [System.Boolean]$ConfigureFirewall = $False
        [System.Boolean]$CodeMeterProtection = $False
        [System.Boolean]$ConfigureVersionSettings = $False
        [System.Boolean]$CopyDejaInsightLibraries = $False
        [string]$ProtekWbcFile = ''


        $CfgFile = Get-IniContent "$CfgIniFile"
        if([string]::IsNullOrEmpty($($CfgFile.PostBuild.ConfigureVersionSettings)) -eq $False){
            [System.Boolean]$ConfigureVersionSettings = "$($CfgFile.PostBuild.ConfigureVersionSettings)" -eq "1"
        }    
        if([string]::IsNullOrEmpty($($CfgFile.PostBuild.CodeMeterProtection)) -eq $False){
            [System.Boolean]$CodeMeterProtection = "$($CfgFile.PostBuild.CodeMeterProtection)" -eq "1"
        }
        if([string]::IsNullOrEmpty($($CfgFile.PostBuild.CopyDejaInsightLibraries)) -eq $False){
            [System.Boolean]$CopyDejaInsightLibraries = "$($CfgFile.PostBuild.CopyDejaInsightLibraries)" -eq "1"
        }    
        if([string]::IsNullOrEmpty($($CfgFile.PostBuild.ProtekWbcFile)) -eq $False){
            $ProtekWbcFile = $ExecutionContext.InvokeCommand.ExpandString($($CfgFile.PostBuild.ProtekWbcFile))
            $ProtekWbcFile = (Resolve-Path -Path "$ProtekWbcFile" -ErrorAction Ignore).Path
            $IsProtekWbcPresent = [System.IO.File]::Exists($ProtekWbcFile)
        }
        if([string]::IsNullOrEmpty($($CfgFile.PostBuild.ProductId)) -eq $False){
            [String]$ProductId = $($CfgFile.PostBuild.ProductId)
        }
        
        Write-Line
        Write-BuildOutTitle "POST BUILD OPERATION"
        Write-Output "`tCopyDejaInsightLibs   $CopyDejaInsightLibraries"
        Write-Output "`tVersionSettings       $ConfigureVersionSettings"
        Write-Output "`tCodeMeterProtection   $CodeMeterProtection"
        Write-Line


        ########################################################################################
        #
        #  UPDATE VERSION STRING
        #
        ########################################################################################

        $HeadRev  = git log --format=%h -1 | select -Last 1
        $LastRev  = git log --format=%h -2 | select -Last 1
        $HeadRevisionLong = git log --format=%H -1

        [string]$VersionString = $($CfgFile.Version.Version)
        [Version]$CurrentVersion = $VersionString
        [Version]$NewVersion = $VersionString
        $NewVersionRevision = $NewVersion.Revision
        $NewVersionRevision++
        $NewVersion = New-Object -TypeName System.Version -ArgumentList $NewVersion.Major,$NewVersion.Minor,$NewVersion.Build,$NewVersionRevision
        [string]$NewVersionString = $NewVersion.ToString()
        
        $CfgFile.Version.MD5 = $MD5HASH
        $CfgFile.Version.Version = $NewVersionString
        $CfgFile.Version.Revision = $HeadRevisionLong 
        $UpdatedVersion = $NewVersionString

        $inf = Get-Item "$BuiltBinary"
        [DateTime]$LastWriteDate = $inf.LastWriteTime
        [String]$LastWriteTime = $LastWriteDate.GetDateTimeFormats()[26]
        $CfgFile.Version.LastBuildTime = $LastWriteTime
      
        Out-IniFile -Path "$CfgIniFile" -InputObject $CfgFile

        ########################################################################################
        #
        #  CONFIGURE VERSION STRINGS IN BINARY
        #
        ########################################################################################

        if ($ConfigureVersionSettings -eq $True) {
            Write-BuildOutTitle "CONFIGURE VERSION SETTINGS"

            Write-Output "`tFileName              $($CfgFile.General.Filename)"
            Write-Output "`tProductName           $($CfgFile.General.ProductName)"
            Write-Output "`tCompanyName           $($CfgFile.General.CompanyName)"
            Write-Output "`tFileDescription       $($CfgFile.General.FileDescription)"
            Write-Output "`tLegalCopyright        $($CfgFile.General.LegalCopyright)"
            Write-Output "`tPrivateBuild          $($CfgFile.General.PrivateBuild)"
            [String]$Description = ""
            if([string]::IsNullOrEmpty($($CfgFile.General.FileDescription)) -eq $False){
                [String]$Description = $($CfgFile.General.FileDescription)
            }
            Set-BinaryFileVersionSettings -Path "$BuiltBinary" -Description "$Description" -VersionString "$NewVersion"
            Set-BinaryFileVersionProperty -Path "$BuiltBinary" -PropertyName "company" -PropertyValue "$($CfgFile.General.CompanyName)"  
            Set-BinaryFileVersionProperty -Path "$BuiltBinary" -PropertyName "copyright" -PropertyValue "$($CfgFile.General.LegalCopyright)"  
            Set-BinaryFileVersionProperty -Path "$BuiltBinary" -PropertyName "LegalTrademarks" -PropertyValue "$($CfgFile.General.LegalTrademarks)"  

            if($Configuration -match "Debug") { 
                Set-BinaryFileVersionProperty -Path "$BuiltBinary" -PropertyName "PrivateBuild" -PropertyValue "$($CfgFile.General.PrivateBuild)"  
            }

        }


        [System.Boolean]$IsAdministrator = Invoke-IsAdministrator 


        ########################################################################################
        #
        #  CodeMeter Protection
        #
        ########################################################################################

        if ( ($IsProtekWbcPresent -eq $True) -And ($IsAdministrator -eq $True) -And ($CodeMeterProtection -eq $True) ){
            Invoke-CodeMeterProtection -TemplateWbc "$ProtekWbcFile" -InputFile "$BuiltBinary" -OutputFile "$BuiltBinary" -ProductId "$ProductId"
        }

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


        ########################################################################################
        #
        #  Copy Config File
        #
        ########################################################################################
        Write-BuildOutTitle "COPY CONFIG FILE ($Platform)"
        Copy-Item $CfgIniFile $OutputDirectory -Force
        Write-Output "[COPY CONFIG] `"$CfgIniFile`" ==> `"$OutputDirectory`""
            
        
        ########################################################################################
        #
        #  Compiled Binary Statistics
        #
        ########################################################################################

        Update-BinaryStatistics "$BuiltBinary" "$StatsFile"


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