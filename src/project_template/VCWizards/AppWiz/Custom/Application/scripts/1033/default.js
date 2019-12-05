// Copyright (c) Microsoft Corporation. All rights reserved.

function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol("PROJECT_PATH");
		var strProjectName = wizard.FindSymbol("PROJECT_NAME");

		var bEmptyProject = wizard.FindSymbol("EMPTY_PROJECT");

		selProj = CreateProject(strProjectName, strProjectPath);
		selProj.Object.Keyword = "Win32Proj";

		AddCommonConfig(selProj, strProjectName);
		var strAppType = GetAppType();
		AddSpecificConfig(selProj, strProjectName, bEmptyProject, strAppType);

		SetupFilters(selProj);

		if (!bEmptyProject)
		{
			if (IsDBCSCharSet(wizard))
				wizard.AddSymbol("ABOUTBOX_FONT_SIZE", "9"); //VS-supported codedepages that require FontSize 9
			else
				wizard.AddSymbol("ABOUTBOX_FONT_SIZE", "8"); //all the rest and unsupported codepages: Fontsize 8 
			
			var Pch = wizard.FindSymbol("PRE_COMPILED_HEADER");

			if ((strAppType == "LIB" || ((strAppType == "CONSOLE") && 
		                !wizard.FindSymbol("SUPPORT_MFC") && !wizard.FindSymbol("SUPPORT_ATL"))) && !Pch) 
			{
				AddFilesToProjectWithInfFile(selProj, strProjectName);
				SetNoPchSettings(selProj);
			}
			else
			{
				AddFilesToProjectWithInfFile(selProj, strProjectName);
				SetCommonPchSettings(selProj);	
			}

			if (strAppType == "CONSOLE")
			{
				var config = selProj.Object.Configurations("Debug");
				var LinkTool = config.Tools("VCLinkerTool");
				if (wizard.FindSymbol("IS_EXPRESS_SKU"))
				{
    					LinkTool.AdditionalDependencies = "kernel32.lib $(NoInherit)";
    				}

				config = selProj.Object.Configurations("Release");
				LinkTool = config.Tools("VCLinkerTool");
				if (wizard.FindSymbol("IS_EXPRESS_SKU"))
				{
					LinkTool.AdditionalDependencies = "kernel32.lib $(NoInherit)";
    				}
			}

		}
		selProj.Object.Save();
	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function SetFileProperties(projfile, strName)
{
	return false;
}

function GetTargetName(strName, strProjectName, strResPath, strHelpPath)
{
	try
	{
		var strTarget = strName;

		if (strName == "readme.txt")
			strTarget = "ReadMe.txt";
		if (strName == "resource.h")
			strTarget = "Resource.h";

		if (strName == "small.ico")
		{
			strTarget = "small.ico";
		}
		if (strName.substr(0, 4) == "root")
		{
			strTarget = strProjectName + strName.substr(4);

		}
		return strTarget; 
	}
	catch(e)
	{
		throw e;
	}
}

function GetAppType()
{
	try
	{
		var strAppType = "";
		if (wizard.FindSymbol("CONSOLE_APP"))
			strAppType = "CONSOLE";
		else
		{
			if (wizard.FindSymbol("WIN_APP"))
				strAppType = "WIN";
			else
			{
				if (wizard.FindSymbol("DLL_APP"))
					strAppType = "DLL";
				else
				{
					if (wizard.FindSymbol("LIB_APP"))
						strAppType = "LIB";
				}
			}
		}
		return strAppType;
	}
	catch(e)
	{
		throw e;
	}
}

function AddSpecificConfig(proj, strProjectName, bEmptyProject, strAppType)
{
	try
	{
		var bMFC = wizard.FindSymbol("SUPPORT_MFC");
		var bATL = wizard.FindSymbol("SUPPORT_ATL");
        
		var config = proj.Object.Configurations("Debug");
		config.CharacterSet = charSetUNICODE;

		if (strAppType == "LIB")
			config.ConfigurationType = typeStaticLibrary;
		else if (strAppType == "DLL")
			config.ConfigurationType = typeDynamicLibrary;

		var CLTool = config.Tools("VCCLCompilerTool");
	//	CLTool.PrecompiledHeaderFile = "$(OutDir)/" + strProjectName + ".pch";

		CLTool.RuntimeLibrary = rtMultiThreadedDebugDLL;

		var strDefines = CLTool.PreprocessorDefinitions;
		if (strDefines != "") strDefines += ";";
		strDefines += GetPlatformDefine(config);
		strDefines += "_DEBUG";

		switch(strAppType)
		{
			case "CONSOLE":
				strDefines += ";_CONSOLE";
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				if (bATL)
					config.UseOfATL = useATLStatic;
				break;
			case "LIB":
				strDefines += ";_LIB";
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				break;
			case "DLL":
				strDefines += ";_WINDOWS;_USRDLL;";
				var strExports = wizard.FindSymbol("UPPER_CASE_SAFE_PROJECT_IDENTIFIER_NAME") + "_EXPORTS";
				strDefines += strExports;
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				if (bATL)
					config.UseOfATL = useATLStatic;
				break;
			case "WIN":
				strDefines += ";_WINDOWS";
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				if (bATL)
					config.UseOfATL = useATLStatic;
				break;
			default:
				break;
		}

		CLTool.PreprocessorDefinitions = strDefines;
		if (bEmptyProject)
			CLTool.UsePrecompiledHeader = pchNone;

		CLTool.DebugInformationFormat = debugEditAndContinue;

		if (strAppType != "LIB")
		{
			var LinkTool = config.Tools("VCLinkerTool");
			LinkTool.GenerateDebugInformation = true;
			LinkTool.LinkIncremental = linkIncrementalYes;

			if (strAppType == "DLL" || strAppType == "WIN")
				LinkTool.SubSystem = subSystemWindows;
			else
				LinkTool.SubSystem = subSystemConsole;
		}

		config = proj.Object.Configurations.Item("Release");
		config.CharacterSet = charSetUNICODE;

		if (strAppType == "LIB")
			config.ConfigurationType = typeStaticLibrary;
		else if (strAppType == "DLL")
			config.ConfigurationType = typeDynamicLibrary;

		var CLTool = config.Tools("VCCLCompilerTool");

		CLTool.RuntimeLibrary = rtMultiThreadedDLL;

		var strDefines = CLTool.PreprocessorDefinitions;
		if (strDefines != "") strDefines += ";";
		strDefines += GetPlatformDefine(config);
		strDefines += "NDEBUG";
		if (bEmptyProject)
			CLTool.UsePrecompiledHeader = pchNone;

		CLTool.DebugInformationFormat = debugEnabled;

		switch(strAppType)
		{
			case "CONSOLE":
				strDefines += ";_CONSOLE";
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				if (bATL)
					config.UseOfATL = useATLStatic;
				break;
			case "LIB":
				strDefines += ";_LIB";
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				break;
			case "DLL":
				strDefines += ";_WINDOWS;_USRDLL;";
				var strExports = wizard.FindSymbol("UPPER_CASE_SAFE_PROJECT_IDENTIFIER_NAME") + "_EXPORTS";
				strDefines += strExports;
				break;
			case "WIN":
				strDefines += ";_WINDOWS";
				if (bMFC)
					config.UseOfMFC = useMfcDynamic;
				if (bATL)
					config.UseOfATL = useATLStatic;
				break;
			default:
				break;
		}

		CLTool.PreprocessorDefinitions = strDefines;

		if (strAppType != "LIB")
		{
			var LinkTool = config.Tools("VCLinkerTool");
			LinkTool.GenerateDebugInformation = true;
			LinkTool.LinkIncremental = linkIncrementalNo;

			if (strAppType == "DLL" || strAppType == "WIN")
				LinkTool.SubSystem = subSystemWindows;
			else
				LinkTool.SubSystem = subSystemConsole;
		}
	}
	catch(e)
	{
		throw e;
	}
}

