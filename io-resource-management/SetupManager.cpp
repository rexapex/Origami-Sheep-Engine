#include "SetupManager.h"


namespace osrm
{
	SetupManager::SetupManager()
	{

	}


	SetupManager::~SetupManager()
	{

	}




	const OSError SetupManager::buildVertShader(std::stringstream & outTxt, std::vector<std::pair<os_uint_t, os_uint_t>> & locs,
		os_bool_t ortho, os_uint_t pos, os_uint_t normals, os_uint_t texCoords, os_uint_t colour,
		os_bool_t instanced, os_bool_t shadows, os_bool_t skinned, os_int_t maxNumBones, os_bool_t normalMapping)
	{
		std::vector<std::pair<os_uint_t, os_uint_t>> inVars, outVars;
		std::vector<os_uint_t> defaultFuncs;
		std::vector<std::string> uniformTypes, uniformNames, customFuncs, defines;

		uniformTypes.push_back("mat4");
		uniformNames.push_back("camToClipMat");

		if(pos > 0 && pos <= 4)
			inVars.push_back(std::make_pair(0, pos));
		if(normals > 0 && normals <= 4)
			inVars.push_back(std::make_pair(1, normals));
		if(texCoords > 0 && texCoords <= 4)
			inVars.push_back(std::make_pair(2, texCoords));
		if(colour > 0 && colour <= 4)
			inVars.push_back(std::make_pair(7, colour));
		if(instanced)
			inVars.push_back(std::make_pair(3, 0));
		if(shadows)
		{
			outVars.push_back(std::make_pair(3, 0));
			uniformTypes.push_back("mat4");
			uniformNames.push_back("lightSpaceMatrix");
		}
		if(skinned)
		{
			inVars.push_back(std::make_pair(4, 0));
			inVars.push_back(std::make_pair(5, 0));
			inVars.push_back(std::make_pair(6, 0));
			std::stringstream ss;
			ss << "MAX_NUM_BONES " << maxNumBones;
			defines.push_back(ss.str());
			uniformTypes.push_back("mat4");
			uniformNames.push_back("boneTransforms[MAX_NUM_BONES]");
		}
		if(normalMapping)
		{
			inVars.push_back(std::make_pair(8, 0));		//tangent
			inVars.push_back(std::make_pair(9, 0));		//bitangent
			outVars.push_back(std::make_pair(4, 0));	//TBN (tangent-bitangent-normal) matrix
		}

		outVars.push_back(std::make_pair(0, 3));
		outVars.push_back(std::make_pair(1, 3));
		outVars.push_back(std::make_pair(2, 2));

		return buildVertShaderGLSL(outTxt, locs, 330, defines, inVars, outVars, uniformTypes, uniformNames, defaultFuncs,
			customFuncs, instanced, shadows, skinned, normalMapping);
	}

	/*In/Out Vectors - Int:
		0 = inPos				 - vec - variable size
		1 = inNormal			 - vec - variable size
		2 = inTexCoords			 - vec - variable size
		3 = inInstanceTransform  - mat4
		4 = inBoneIDs			 - ivec4
		5 = inBoneWeights		 - vec4
		6 = inInstanceBoneOffset - int
		7 = inColour			 - vec - variable size

		0 = vertexCamSpacePos
		1 = vertexNormal
		2 = vertexUV
		3 = fragPosLightSpace
	*/
	const OSError SetupManager::buildVertShaderGLSL(std::stringstream & source, std::vector<std::pair<os_uint_t, os_uint_t>> & locs,
		os_int_t version, const std::vector<std::string> & defines,
		const std::vector<std::pair<os_uint_t, os_uint_t>> & inVars, const std::vector<std::pair<os_uint_t, os_uint_t>> & outVars,
		const std::vector<std::string> & uniformTypes, const std::vector<std::string> & uniformNames,
		const std::vector<os_uint_t> & defaultFuncs, const std::vector<std::string> & customFuncs,
		os_bool_t instanced, os_bool_t shadows, os_bool_t skinned, os_bool_t normalMapping)
	{
		os_bool_t outNormal = false, outTexCoords = false, applyCamToClipTransform = false;




		//Version  Number
		source << "#version " << version << std::endl << std::endl;



		//Defines
		for(size_t i = 0; i < defines.size(); i++)
		{
			source << "#define " << defines[i] << std::endl;
		}

		source << std::endl;



		//Input Variables
		os_uint_t location = 0;
		for(size_t i = 0; i < inVars.size(); i++, location++)
		{
			std::string inType, inName;

			switch(inVars[i].first)
			{
			case 0:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_inPos";
				locs.push_back(std::make_pair(0, location));
				break;
			case 1:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_inNormal";
				locs.push_back(std::make_pair(1, location));
				break;
			case 2:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_inTexCoords";
				locs.push_back(std::make_pair(2, location));
				break;
			case 3:
				inType = "mat4";
				inName = "vert_inInstanceTransform";
				locs.push_back(std::make_pair(3, location));
				break;
			case 4:
				inType = "vec4";
				inName = "vert_inBoneIDs";
				locs.push_back(std::make_pair(4, location));
				break;
			case 5:
				inType = "vec4";
				inName = "vert_inBoneWeights";
				locs.push_back(std::make_pair(5, location));
				break;
			case 6:
				inType = "int";
				inName = "vert_inInstanceBoneOffset";
				locs.push_back(std::make_pair(6, location));
				break;
			case 7:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_inColour";
				locs.push_back(std::make_pair(7, location));
				break;
			default:
				std::cerr << "Unknown variable at inVars[" << i << "]" << std::endl;
				return OSError::SETUP;
			}

			source << "layout(location = " << location << ") in " << inType << " " << inName << ";" << std::endl;

			if(inVars[i].first == 3)	location += 3;	//inInstanceTransform takes 3 extra location as it is 4 vec4s
		}
		
		source << std::endl;



		//Output Variables
		for(size_t i = 0; i < outVars.size(); i++)
		{
			std::string outType, outName;

			switch(outVars[i].first)
			{
			case 0:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "vert_outVertPos";
				break;
			case 1:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "vert_outVertNormal";
				outNormal = true;
				break;
			case 2:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "vert_outVertTexCoords";
				outTexCoords = true;
				break;
			case 3:
				outType = "vec4";
				outName = "fragPosLightSpace";
			case 7:
				outType = "vec4";
				outName = "vert_outVertColour";	//TODO - Set vert_outVertColour - Colour Not Yet Supported
				break;
			default:
				std::cerr << outVars[i].first << ", " << outVars[i].second << "\n";
				std::cerr << "BUILD VERT SHADER GLSL: Unknown variable at outVars[" << i << "]" << std::endl;
				return OSError::SETUP;
			}

			source << "out " << outType << " " << outName << ";" << std::endl;
		}

		source << std::endl;



		//Uniform Variables
		if(uniformNames.size() != uniformTypes.size())
		{
			std::cerr << "Shader build failed, uniform names size != uniform type extensions size" << std::endl;
		}
		else
		{
			for(size_t u = 0; u < uniformTypes.size(); u++)
			{
				source << "uniform " << uniformTypes[u] << " " << uniformNames[u] << ";" << std::endl;
				if(uniformTypes[u] == "mat4" && uniformNames[u] == "camToClipMat") applyCamToClipTransform = true;
			}
		}

		source << std::endl;



		//Main Function
		source << "void main()" << std::endl << "{" << std::endl;
		source << "    vec3 instancePos;" << std::endl;
		source << "    vec3 instanceNormal;" << std::endl;
		source << std::endl;

		if(skinned)
		{
			source << "    mat4 boneTransform = boneTransforms[int(vert_inBoneIDs[0]) + vert_inInstanceBoneOffset] * vert_inBoneWeights[0]" << std::endl
				   << "         +               boneTransforms[int(vert_inBoneIDs[1]) + vert_inInstanceBoneOffset] * vert_inBoneWeights[1]" << std::endl
				   << "         +               boneTransforms[int(vert_inBoneIDs[2]) + vert_inInstanceBoneOffset] * vert_inBoneWeights[2]" << std::endl
				   << "         +               boneTransforms[int(vert_inBoneIDs[3]) + vert_inInstanceBoneOffset] * vert_inBoneWeights[3];" << std::endl << std::endl;
		}

		if(instanced)
		{
			if(skinned)
			{
				source << "    instancePos = vec3(vert_inInstanceTransform * boneTransform * vec4(vert_inPos, 1.0));" << std::endl;
				source << "    instanceNormal = vec3(transpose(inverse(vert_inInstanceTransform)) * boneTransform * vec4(vert_inNormal, 1.0));" << std::endl;
			}
			else
			{
				source << "    instancePos = vec3(vert_inInstanceTransform * vec4(vert_inPos, 1.0));" << std::endl;
				source << "    instanceNormal = vec3(transpose(inverse(vert_inInstanceTransform)) * vec4(vert_inNormal, 1.0));" << std::endl;
			}
		}
		else
		{
			source << "    instancePos = vert_inPos;" << std::endl;
			source << "    instanceNormal = vert_inNormal;" << std::endl;
		}
		source << std::endl;

		source << "    vert_outVertPos = instancePos;" << std::endl;
		if(shadows)			source << "	   fragPosLightSpace = lightSpaceMatrix * vec4(instancePos, 1.0);" << std::endl;
		if(outNormal)		source << "    vert_outVertNormal = instanceNormal;" << std::endl;
		if(outTexCoords)	source << "    vert_outVertTexCoords = vert_inTexCoords;" << std::endl;
		source << std::endl;

		if(applyCamToClipTransform)
		{
			source << "    gl_Position = camToClipMat * vec4(instancePos, 1.0);" << std::endl;
		}
		else
		{
			source << "    gl_Position = vec4(instancePos, 1.0);" << std::endl;
		}

		source << "}" << std::endl << std::endl;


		return OSError::NONE;
	}




	const OSError SetupManager::buildFragShader(std::stringstream & outTxt, os_bool_t ortho, os_uint_t pos, os_uint_t normals, os_uint_t texCoords,
		os_uint_t colour, os_bool_t deferred, os_bool_t shadows, os_bool_t highDynamicRange, os_bool_t normalMapping)
	{
		std::vector<std::pair<os_uint_t, os_uint_t>> inVars, outVars;
		std::vector<os_uint_t> defaultFuncs;
		std::vector<std::string> uniformTypes, uniformNames, customFuncs, defines;
		os_bool_t materials = false;

		if(pos > 0 && pos <= 4)
			inVars.push_back(std::make_pair(0, pos));
		if(normals > 0 && normals <= 4)
			inVars.push_back(std::make_pair(1, normals));
		if(texCoords > 0 && texCoords <= 4)
		{
			uniformTypes.push_back("sampler2D");
			uniformNames.push_back("texSampler");
			uniformTypes.push_back("Material");
			uniformNames.push_back("material");
			inVars.push_back(std::make_pair(2, texCoords));
			materials = true;
		}
		if(colour > 0 && colour <= 4)
			inVars.push_back(std::make_pair(7, colour));
		if(shadows)
		{
			//outVars.push_back(std::make_pair(3, 0));
			//uniformTypes.push_back("mat4");
			//uniformNames.push_back("lightSpaceMatrix");
		}

		if(highDynamicRange)
		{
			uniformTypes.push_back("float");
			uniformNames.push_back("exposure");
		}

		if(deferred)
		{
			outVars.push_back(std::make_pair(0, 3));	//Pos
			outVars.push_back(std::make_pair(1, 3));	//Normal
			outVars.push_back(std::make_pair(2, 4));	//Colour-Specular
		}
		else
		{
			outVars.push_back(std::make_pair(3, 3));	//Colour
		}

		return buildFragShaderGLSL(outTxt, 330, defines, inVars, outVars, uniformTypes, uniformNames,
			defaultFuncs, customFuncs, deferred, materials, highDynamicRange, normalMapping);
	}

	/*In/Out Vectors - Int:
	0 = vert_outVertPos				 - vec - variable size
	1 = vert_outVertNormal			 - vec - variable size
	2 = vert_outVertTexCoords		 - vec - variable size
	7 = vert_outVertColour			 - vec - variable size

	0 = frag_outFragPos				 - vec3
	1 = frag_outFragNormal			 - vec3
	2 = frag_outFragColourSpec		 - vec4
	3 = frag_outFragColour			 - vec3
	*/
	const OSError SetupManager::buildFragShaderGLSL(std::stringstream & source, os_int_t version, const std::vector<std::string> & defines,
		const std::vector<std::pair<os_uint_t, os_uint_t>> & inVars, const std::vector<std::pair<os_uint_t, os_uint_t>> & outVars,
		const std::vector<std::string> & uniformTypes, const std::vector<std::string> & uniformNames,
		const std::vector<os_uint_t> & defaultFuncs, const std::vector<std::string> & customFuncs,
		os_bool_t deferred, os_bool_t materials, os_bool_t highDynamicRange, os_bool_t normalMapping)
	{
		os_bool_t applyInstanceTransform = false, applyCamToClipTransform = false, outPos = false, outNormal = false, outTexCoords = false,
			boneWeightsPresent = false, boneIDsPresent = false, instanceBoneOffsetPresent = false, boneTransformsPresent = false, shadows = false;




		//Version  Number
		source << "#version " << version << std::endl << std::endl;



		//Defines
		for(size_t i = 0; i < defines.size(); i++)
		{
			source << "#define " << defines[i] << std::endl;
		}

		source << std::endl;



		//Input Variables
		for(size_t i = 0; i < inVars.size(); i++)
		{
			std::string inType, inName;

			switch(inVars[i].first)
			{
			case 0:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_outVertPos";
				break;
			case 1:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_outVertNormal";
				break;
			case 2:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_outVertTexCoords";
				break;
			case 7:
				inType = "vec" + std::to_string(inVars[i].second);
				inName = "vert_outVertColour";
				break;
			default:
				std::cerr << "BUILD FRAG SHADER GLSL: Unknown variable at inVars[" << i << "]" << std::endl;
				return OSError::SETUP;
			}

			source << "in " << inType << " " << inName << ";" << std::endl;
		}

		source << std::endl;



		//Output Variables
		os_uint_t location = 0;
		for(size_t i = 0; i < outVars.size(); i++, location++)
		{
			std::string outType, outName;

			//Should be outVars[i] ? ?!?!?!? ?!? !? ?!?
			switch(outVars[i].first)
			{
			case 0:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "frag_outFragPos";
				break;
			case 1:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "frag_outFragNormal";
				break;
			case 2:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "frag_outFragColourSpec";
				outTexCoords = true;
				break;
			case 3:
				outType = "vec" + std::to_string(outVars[i].second);
				outName = "frag_outFragColour";
				shadows = true;
				break;
			default:
				std::cerr << outVars[i].first << ", " << outVars[i].second << "\n";
				std::cerr << "BUILD FRAG SHADER GLSL: Unknown variable at outVars[" << i << "]" << std::endl;
				return OSError::SETUP;
			}

			source << "layout(location = " << location << ")" << " out " << outType << " " << outName << ";" << std::endl;
		}

		source << std::endl;


		if(materials)
		{
			source << "struct Material" << std::endl
				<<	"{" << std::endl
				<<	"	vec4 ambient;" << std::endl
				<<	"	vec4 diffuse;" << std::endl
				<<	"	vec4 specular;" << std::endl
				<<	"	float shininess;" << std::endl
				<<	"};" << std::endl << std::endl;
		}



		//Uniform Variables
		if(uniformNames.size() != uniformTypes.size())
		{
			std::cerr << "Frag Shader build failed, uniform names size != uniform type extensions size" << std::endl;
		}
		else
		{
			for(size_t u = 0; u < uniformTypes.size(); u++)
			{
				source << "uniform " << uniformTypes[u] << " " << uniformNames[u] << ";" << std::endl;
			}
		}

		source << std::endl;



		//Main Function
		source << "void main()" << std::endl << "{" << std::endl;

		source << "vec3 texColour = texture(texSampler, vert_outVertTexCoords).rgb;" << std::endl;
		if(highDynamicRange)
		{
			//Exposure Tone Mapping
			source << "texColour = vec3(1.0) - exp(-texColour * exposure);" << std::endl;
		}

		if(deferred)
		{
			source << "    frag_outFragPos = vert_outVertPos;" << std::endl;
			source << "    frag_outFragNormal = normalize(vert_outVertNormal);" << std::endl;
			source << "    frag_outFragColourSpec.rgb = texColour;" << std::endl;
			source << "    frag_outFragColourSpec.a = material.shininess;" << std::endl;
		}
		else
		{
			//TODO - Add Support for Non Deferred Rendering
		}

		source << "}" << std::endl << std::endl;



		return OSError::NONE;
	}







	const std::string SetupManager::loadTextFile(const std::string & strShaderFile) const
	{
		const char * cstrShaderFile = strShaderFile.c_str();	//Convert string to C string

		std::string strShaderCode;
		std::ifstream shaderStream(cstrShaderFile, std::ios::in);
		if(shaderStream.is_open())                    //Load the shader source code from the shader file
		{
			std::string line = "";
			while(getline(shaderStream, line))
				strShaderCode += "\n" + line;
			shaderStream.close();
		}

		return strShaderCode;
	}
}
