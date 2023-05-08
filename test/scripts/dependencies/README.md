# PowerShell Build Scripts

Tools to be included in C++ projects...

## Recommended Project Structure

```
	/ProjectRoot
	/ProjectRoot/src
	/ProjectRoot/vs
	/ProjectRoot/scripts
	/ProjectRoot/scripts/dependencies   <-- THESE SCRIPTS
```

So you can do

```
	cd $ProjectRoot
	mkdir scripts
	cd scripts
	git submodule add https://github.com/arsscriptum/Powershell.BuildScripts.git dependencies
	Copy-Item ./dependencies/templates/*.ps1 .
	gpush
	cd ..
	gpush
```


## Add Pre/Post Build Steps with PowerShell Scripts

```
	  <PropertyGroup>
	    <OutDir>$(ProjectDir)..\bin\$(Platform)\$(Configuration)\</OutDir>
	    <IntDir>$(ProjectDir)__build_obj\$(Platform)\$(Configuration)\</IntDir>
	    <PlExePath>$(ProjectDir)..\tools\pl\pl.exe</PlExePath>
	    <LinkIncremental>false</LinkIncremental>
	    <TargetName>$(TargetName)</TargetName>
	    <OutputBinary>$(OutDir)$(TargetName)$(TargetExt)</OutputBinary>
	  </PropertyGroup>
	    <ClCompile>
	    	...
	    </ClCompile>
	    <Link>
	    	...
	    </Link>
	    <PreBuildEvent>
	      <Command>pwsh.exe -nop -noni -f $(ScriptsDirectory)Invoke-PreBuild.ps1 $(TargetName) $(SolutionRootDirectory) $(PlExePath)</Command>
	    </PreBuildEvent>
	    <PostBuildEvent>
	      <Command>pwsh.exe -nop -noni -f $(ScriptsDirectory)Invoke-ServerPostBuild.ps1 $(Configuration) $(Platform) $(SolutionRootDirectory) $(OutputBinary) $(UseDebugScriptOutput)</Command>
	    </PostBuildEvent>
	  </ItemDefinitionGroup>
```

## CODEMETER / BINARY PROTECTION

In Progress

## VERSION SETTINGS

In Progress

Depends On ```https://github.com/arsscriptum/Native.VersionPatcher.git```

```
    Get-VersionPatcherProgram in VersionPatcher.ps1
```

## ServiceInstaller.ps1 and ServicePermission.ps1

In Progress

## FirewallConfig

In Progress

## Statistics

In Progress
