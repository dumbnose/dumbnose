// Copyright (c) Microsoft Corporation. All rights reserved.
function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol("PROJECT_PATH");
		var strProjectName = wizard.FindSymbol("PROJECT_NAME");
		
		selProj = CreateProject(strProjectName, strProjectPath);
		selProj.Object.Keyword = "MakeFileProj";
		
		var bSameAsDebug = wizard.FindSymbol("RELEASE_SAME_AS_DEBUG");
		
		var debugConfig = selProj.Object.Configurations("Debug");
		SetConfigProperties(debugConfig, true /* debug */, bSameAsDebug, strProjectName);

		var releaseConfig = selProj.Object.Configurations("Release");
		SetConfigProperties(releaseConfig, false /* not debug */, bSameAsDebug, strProjectName);
		
		SetupFilters(selProj);
		
	    RenderAddTemplate(wizard, "readme.txt", "readme.txt", selProj.ProjectItems, false);

		selProj.Object.Save();

	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function GetPreprocessorDefines(config, strUserDefines, bDebug)
{
	var strDefines = GetPlatformDefine(config);
	
	if (bDebug)
		strDefines += "_DEBUG";
	else
		strDefines += "NDEBUG";
	
	if (strUserDefines.length > 0)
		return strUserDefines + ";" + strDefines;
	
	return strDefines;
}

function SetConfigProperties(config, bDebug, bSameAsDebug, strProjectName)
{
	try
	{
		config.OutputDirectory = "$(ConfigurationName)";
		config.IntermediateDirectory = "$(ConfigurationName)";
		config.ConfigurationType = typeUnknown;

		var strUserDefines = wizard.FindSymbol((bDebug || bSameAsDebug) ? "PREPROCESSOR_DEFINES_DEBUG" : "PREPROCESSOR_DEFINES");
		strUserDefines = GetPreprocessorDefines(config, strUserDefines, bDebug);
		
		var strIncludeSearchPath = wizard.FindSymbol((bDebug || bSameAsDebug) ? "INCLUDE_PATH_DEBUG" : "INCLUDE_PATH");
		var strForcedIncludes = wizard.FindSymbol((bDebug || bSameAsDebug) ? "FORCED_INCLUDED_FILES_DEBUG" : "FORCED_INCLUDED_FILES");
		var strAssemblySearchPath = wizard.FindSymbol((bDebug || bSameAsDebug) ? "ASSEMBLY_PATH_DEBUG" : "ASSEMBLY_PATH");
		var strForcedUsingAssemblies = wizard.FindSymbol((bDebug || bSameAsDebug) ? "FORCED_USING_ASSEMBLIES_DEBUG" : "FORCED_USING_ASSEMBLIES");
		
		var strCommandLine = wizard.FindSymbol((bDebug || bSameAsDebug) ? "BUILD_COMMAND_LINE_DEBUG" : "BUILD_COMMAND_LINE");
		var strCleanCommands = wizard.FindSymbol((bDebug || bSameAsDebug) ? "CLEAN_COMMAND_LINE_DEBUG" : "CLEAN_COMMAND_LINE");
		var strRebuildCommandLine = wizard.FindSymbol((bDebug || bSameAsDebug) ? "REBUILD_COMMAND_LINE_DEBUG" : "REBUILD_COMMAND_LINE");
		var strOutput = wizard.FindSymbol((bDebug || bSameAsDebug) ? "OUTPUT_DEBUG" : "OUTPUT");
			
		var NMakeTool = config.Tools("VCNMakeTool");
		if (strCommandLine.length)
			NMakeTool.BuildCommandLine = strCommandLine;
		if (strOutput.length)
			NMakeTool.Output = strOutput;
		if (strCleanCommands.length)
			NMakeTool.CleanCommandLine = strCleanCommands;
		if (strRebuildCommandLine.length)
			NMakeTool.ReBuildCommandLine = strRebuildCommandLine;
		if (strUserDefines.length)
			NMakeTool.PreprocessorDefinitions = strUserDefines;
		if (strIncludeSearchPath.length)
			NMakeTool.IncludeSearchPath = strIncludeSearchPath;
		if (strForcedIncludes.length)
			NMakeTool.ForcedIncludes = strForcedIncludes;
		if (strAssemblySearchPath.length)
			NMakeTool.AssemblySearchPath = strAssemblySearchPath;
		if (strForcedUsingAssemblies.length)
			NMakeTool.ForcedUsingAssemblies = strForcedUsingAssemblies;
	}
	catch(e)
	{
		throw e;
	}
}

