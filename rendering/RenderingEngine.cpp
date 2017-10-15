/*
 * RenderingEngine.cpp
 *
 *  Created on: 5 Oct 2015
 *      Author: james_000
 */

#include "RenderingEngine.h"


namespace osr
{
	RenderingEngine::RenderingEngine(os_int_t framebufferWidth, os_int_t framebufferHeight)
	{
	///	mainCamTransform = new osm::Transform;
	///	osm::translate(mainCamTransform, glm::vec3(0.0f, 0.0f, -1.0f));
		mainCamTransform = nullptr;

		matUBO = 0;
		lightUBO = 0;
		skyboxTexID = 0;

		useMIPMapping = true;
		texFilterMode = TexFilterMode::ANISOTROPIC_MIP_MAP_LINEAR;
		anisotropicLevel = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropicLevel);

		std::cerr << "Max Anisotropy: " << anisotropicLevel << std::endl;

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_2D);


		perspectiveProjection  = genPerspectiveMatrix(glm::radians(50.0f), framebufferWidth, framebufferHeight);
		orthographicProjection = glm::ortho(0.0f, (GLfloat)framebufferWidth, 0.0f, (GLfloat)framebufferHeight);
		lightProjection		   = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
		this->framebufferWidth = framebufferWidth;
		this->framebufferHeight = framebufferHeight;
		framebufferX = 0;
		framebufferY = 0;
		fovRadians = glm::radians(50.0f);

		uboIDMatrices = genUBO(sizeof(glm::mat4));	//glm::mat4 has same size as glsl mat4
		os_uint_t blockIndexMatrices = 0;						//block index ~= uniform location
		os_uint_t bindingPointMatrices = 0;
		glBindBuffer(GL_UNIFORM_BUFFER, uboIDMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(perspectiveProjection));	//First mat4 so offest = 0 and size = sizeof(mat4)
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointMatrices, uboIDMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		




		/*float sharpenKernel[9] =
		{
			-1.0f, -1.0f, -1.0f,
			-1.0f,  9.0f, -1.0f,
			-1.0f, -1.0f, -1.0f
		};
		float blurKernel[9] =
		{
			1.0f/16, 2.0f/16, 1.0f/16,
			2.0f/16, 4.0f/16, 2.0f/16,
			1.0f/16, 2.0f/16, 1.0f/16
		};
		float godraysKernel[9] =
		{
			1, 1, 1,
			1, 8, 1,
			1, 1, 1
		};
		float edgeDetectionKernel[9] =
		{
			1, 1, 1,
			1, -8, 1,
			1, 1, 1
		};*/






		/*useVsync = true;
		useMIPMapping = true;
		useTripleBuffering = false;
		antiAliasingMode = NONE;
		brightness = 1.0f;
		textureQuality = LOW;
		meshQuality = LOW;*/

		glClearColor(0, 0, 0, 0);	//Set initial values in colour buffer
		glClearDepth(1.0f);			//Set up initial values in depth buffer

		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	RenderingEngine::~RenderingEngine()
	{
		// TODO Auto-generated destructor stub
	}






	//Renders the group of renderables in each group of shaders
	void RenderingEngine::render()
	{
		/*glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightSpaceMatrix = glm::mat4();

		glUseProgram(shadowMapProg);
		for(size_t l = 0; l < dirLights->size(); l++)
		{
			glm::mat4 lightView = glm::lookAt(glm::vec3(-(*dirLights)[l].dir),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			lightSpaceMatrix = lightProjection * lightView;
			glUniformMatrix4fv(shadowMapPos0, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

			for(size_t s = 0; s < shaderGroups.size(); s++)	//For each shader group in shader groups
			{
				const ShaderGroup shaderGroup = shaderGroups[s];

				for(size_t o = 0; o < shaderGroup.renderObjects.size(); o++)	//For each object in shader group
				{
					RenderObject renderObject = shaderGroup.renderObjects[o];

					glBindVertexArray(renderObject.VAO);

					for(size_t t = 0; t < renderObject.transforms.size(); t++)
					{
						glUniformMatrix4fv(shadowMapPos1, 1, GL_FALSE, glm::value_ptr(*renderObject.transforms[t]));

						if(renderObject.IBO == 0)	//If there are no IBOs
						{
							glDrawArrays(GL_TRIANGLES, 0, renderObject.noVerticesPerRenderCall[0]);	//Then render all vertices in the vbo
						} else
						{
							os_sizeiptr_t offset = 0;
							for(size_t r = 0; r < renderObject.sizeNoVerticesPerRenderCall; r++)
							{
								glDrawElementsInstanced(GL_TRIANGLES, renderObject.noVerticesPerRenderCall[r], GL_UNSIGNED_INT, (GLvoid *)offset, (GLsizei)renderObject.numInstances);
								offset += renderObject.noVerticesPerRenderCall[r] * sizeof(os_uint_t);
							}
						}
					}
					glBindVertexArray(0);
				}
			}
		}





		glViewport(0, 0, framebufferWidth, framebufferHeight);*/
		//glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);

		//std::cerr << "No. Shader Groups: " << shaderGroups.size() << std::endl;



		//Update light positions relative to main camera transform
		//TODO - Replace with more efficient system
		glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
		os_uint_t sizeVec4 = sizeof(glm::vec4);
		for(size_t t = 0; t < (*pointLights).size(); t++)	//For each instance of the object to be rendered
		{
			//Multiply the transform by the world to cam mat and sub it into the vbo
			glm::vec4 camSpacePos = glm::inverse(mainCamTransform->modelMatrix) * (*pointLights)[t].relativePos;
			glBufferSubData(GL_UNIFORM_BUFFER, t * sizeof(PointLight), sizeVec4, &camSpacePos);
		}
		for(size_t t = 0; t < (*dirLights).size(); t++)	//For each instance of the object to be rendered
		{
			//Multiply the transform by the world to cam mat and sub it into the vbo
			glm::vec4 camSpaceDir = glm::inverse(mainCamTransform->orientation) * (*dirLights)[t].dir;
			glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(PointLight), sizeVec4, &camSpaceDir);
		}
		for(size_t t = 0; t < (*spotLights).size(); t++)	//For each instance of the object to be rendered
		{
			//Multiply the transform by the world to cam mat and sub it into the vbo
			glm::vec4 camSpacePos = glm::inverse(mainCamTransform->modelMatrix) * (*spotLights)[t].relativePos;
			glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(PointLight) + sizeof(DirectionLight) + t * sizeof(SpotLight), sizeVec4, &camSpacePos);
			glm::vec4 camSpaceDir = (mainCamTransform->orientation) * (*spotLights)[t].dir;
			glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(PointLight) + sizeof(DirectionLight) + t * sizeof(SpotLight) + sizeof(glm::vec4), sizeVec4, &camSpaceDir);
		}




		for(size_t rp = 0; rp < renderPasses.size(); rp++)
		{
			RenderPass & renderPass = renderPasses[rp];

			//glViewport(0, 0, renderPass.FBOWidth, renderPass.FBOHeight);
			renderPass.blend	 ? glEnable(GL_BLEND)	   : glDisable(GL_BLEND);
			renderPass.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, renderPass.FBO);
			glClear(renderPass.clear);

			for(size_t s = 0; s < renderPass.shaderGroups.size(); s++)	//For each shader group in shader groups
			{
				ShaderGroup & shaderGroup = renderPass.shaderGroups[s];

				glBindBuffer(GL_UNIFORM_BUFFER, shaderGroup.UBO);

				size_t p = 0;
				do	//Always do one render, even if there are no shaders
				{
					ShaderProgram & shaderProgram = shaderGroup.shaderPrograms[p];

					glUseProgram(shaderProgram.program);				//Bind the shader program


					///Light Position in camera space
					///glUniformMatrix4fv(shaderGroups[s].shaderPrograms[p].uniforms[2], 1, GL_FALSE, glm::value_ptr(glm::vec3(-10, 0, 5)));

					for(size_t o = 0; o < shaderGroup.renderObjects.size(); o++)	//For each object in shader group
					{
						RenderObject & renderObject = shaderGroup.renderObjects[o];

						///std::cerr << "Instance VBO: " << renderObject.instanceVBO << std::endl;
						if(renderObject.instanceVBO != 0)
						{
							glBindBuffer(GL_ARRAY_BUFFER, renderObject.instanceVBO);

							os_uint_t sizeMat4 = sizeof(glm::mat4);
							for(size_t t = 0; t < renderObject.transforms.size(); t++)	//For each instance of the object to be rendered
							{
								//Multiply the transform by the world to cam mat and sub it into the vbo
								renderObject.camSpaceTransforms[t] = glm::inverse(mainCamTransform->modelMatrix) * (*renderObject.transforms[t]);
							}
							if(renderObject.camSpaceTransforms.size() > 0)
								glBufferSubData(GL_ARRAY_BUFFER, 0, renderObject.camSpaceTransforms.size() * sizeMat4, &renderObject.camSpaceTransforms[0]);
						}


						glBindVertexArray(renderObject.VAO);



						//Inverse Transpose of matrix should be used for normModelToCamMat if matrix is non-uniform
						///glm::mat4 normModelToCamMat(glm::inverse(mainCamTransform->modelMatrix));
						///normModelToCamMat = glm::transpose(glm::inverse(normModelToCamMat));
						///glUniformMatrix4fv(shaderProgram.uniforms[4], 1, GL_FALSE, glm::value_ptr(normModelToCamMat));

						//Light Space Matrix
						/*glm::mat4 biasMatrix(
							0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0
						);
						glUniformMatrix4fv(shaderProgram.uniforms[5], 1, GL_FALSE, glm::value_ptr(biasMatrix * lightSpaceMatrix));
						glActiveTexture(GL_TEXTURE0 + 1);
						glBindTexture(GL_TEXTURE_2D, shadowMap);*/



						//Bind all the uniforms required by the render object
						for(size_t u = 0; u < renderObject.uniforms.size(); u++)
						{
							uniform(renderObject.uniforms[u].func, renderObject.uniforms[u].loc, renderObject.uniforms[u].data);
						}

						if(renderObject.IBO == 0)	//If there are no IBOs
						{
							if(renderObject.sizeTextures > 0 && renderObject.sizeNumTexturesPerRenderCall > 0)
							{
								for(size_t t = 0; t < renderObject.numTexturesPerRenderCall[0]; t++)
								{
								///	std::cerr << "TEXTURE: " << renderObject.textures[t].type << ", " << renderObject.textures[t].texID << std::endl;
									glActiveTexture(GL_TEXTURE0 + t);
									glBindTexture(renderObject.textures[t].type, renderObject.textures[t].texID);
								}
							}

							glDrawArrays(renderObject.renderPrimitive, 0, renderObject.numVerticesPerRenderCall[0]);	//Then render all vertices in the vbo
						} else
						{
							os_sizeiptr_t offset = 0, textureOffset = 0;
							for(size_t r = 0; r < renderObject.sizeNumVerticesPerRenderCall; r++)
							{
								if(r < renderObject.sizeNumTexturesPerRenderCall)	//Test whether there is a texture for this render call - Only works if padding is enabled
								{
									for(size_t t = 0; t < renderObject.numTexturesPerRenderCall[r]; t++)
									{
									///	std::cerr << "TEXTURE: " << renderObject.textures[textureOffset + t].type << ", " << renderObject.textures[textureOffset + t].texID << std::endl;
										glActiveTexture(GL_TEXTURE0 + t);
										glBindTexture(renderObject.textures[t].type, renderObject.textures[textureOffset + t].texID);
									}

									textureOffset += renderObject.numTexturesPerRenderCall[r];
								}

								glDrawElementsInstanced(renderObject.renderPrimitive, renderObject.numVerticesPerRenderCall[r], GL_UNSIGNED_INT, (GLvoid *)offset, (GLsizei)renderObject.numInstances);

								offset += renderObject.numVerticesPerRenderCall[r] * sizeof(os_uint_t);
							}
						}
						glBindVertexArray(0);
					}
					p++;
				} while(p < shaderGroup.shaderPrograms.size());	//Same objects can be rendered multiple times with different shaders
			}
		}
	}




	void RenderingEngine::addRenderable(size_t renderPassIndex, const osr::ShaderGroup & shaderGroup)
	{
		bool commonShaderGroupExists = false;

		if(renderPassIndex >= renderPasses.size())
			return;

		for(RenderObject r : shaderGroup.renderObjects)	//For each render object in the shader group...
		{
			GLenum p = r.renderPrimitive;

			//...if the primitive render type is invalid, then set the primitive render type to GL_TRIANGLES
			if(!(p == GL_POINTS || p == GL_LINES || p == GL_LINE_LOOP || p == GL_LINE_STRIP || p == GL_TRIANGLES || p == GL_TRIANGLE_STRIP ||
				p == GL_TRIANGLE_FAN || p == GL_LINES_ADJACENCY || p == GL_LINE_STRIP_ADJACENCY || p == GL_TRIANGLES_ADJACENCY ||
				p == GL_TRIANGLE_STRIP_ADJACENCY || p == GL_QUADS || p == GL_QUAD_STRIP))
			{
				r.renderPrimitive = GL_TRIANGLES;
			}
		}

		std::vector<ShaderGroup> & shaderGroups = renderPasses[renderPassIndex].shaderGroups;

		for(size_t i = 0; i < shaderGroups.size(); i++)	//For every already existing shader group...
		{
			commonShaderGroupExists = true;

			for(size_t k = 0; k < shaderGroups[i].shaderPrograms.size(); k++)
			{
				if(shaderGroup.shaderPrograms[k].program != shaderGroups[i].shaderPrograms[k].program)	//If shader groups contain different shaders, break from the loop
				{
					commonShaderGroupExists = false;
					break;
				}
			}

			if(commonShaderGroupExists)
			{
				for(size_t j = 0; j < shaderGroup.renderObjects.size(); j++)
				{
					//shaderGroup.renderObjects[j].entityID = 0;//nextEntityID++
					shaderGroups[i].renderObjects.push_back(shaderGroup.renderObjects[j]);	//Then add each renderable to the existing common shader group
					commonShaderGroupExists = true;
					break;
				}
				break;
			}
		}

		if(!commonShaderGroupExists)	//If there is no common shader group
		{
			std::cerr << "Gen new Shader Group" << std::endl;
			shaderGroups.push_back(shaderGroup);	//Add the shader group to the shader groups list
		}
	}

	void RenderingEngine::addRenderPass(const osr::RenderPass & renderPass)
	{
		renderPasses.push_back(renderPass);
	}






	void RenderingEngine::updateSkinnedAnimations(osan::AnimationEngine * animationEngine)	//Send bones transforms to skinned mesh shaders
	{
		os_int_t offset = 0;

		for(size_t a = 0; a < animationEngine->getAnimationControllers().size(); a++)
		{
			const osan::AnimationController & animController = animationEngine->getAnimationController(a);
			const osan::SkinnedMesh & skinnedMesh = animController.skinnedMesh;

			os_uint_t shaderProg = skinnedMesh.shaderProg;
			glUseProgram(shaderProg);

			for(size_t i = 0; i < skinnedMesh.bones.size(); i++)
			{
				std::string locName("boneTransforms[");
				std::stringstream ss;
				///std::cerr << "i: " << i << ", SKINNED MESH INDEX: " <<  animController.skinnedMeshIndex << ", OFFSET: " << i + animController.skinnedMeshIndex * skinnedMesh.bones.size() << std::endl;
				///std::cerr <<"BONE BUFFER OFFSET: " << skinnedMesh.boneBufferOffset << "\n";
				///ss << i + 0 * skinnedMesh.bones.size();
				ss << (skinnedMesh.totalBoneOffset + i + animController.skinnedMeshIndex * skinnedMesh.bones.size());	//Offset into the array by the instance number
				locName += ss.str();
				locName += "]";

				glUniformMatrix4fv(glGetUniformLocation(shaderProg, locName.c_str()), 1, GL_FALSE, glm::value_ptr(skinnedMesh.bones[i].finalMatrix));
			}
		}
	}

	void RenderingEngine::updateUserInterface(osi::UserInterface * userInterface)	//Update user interface shaders on resize of framebuffer
	{
		shouldUpdateUI = false;

		os_int_t startIndex = updateUIStartIndex;
		os_int_t endIndex = updateUIEndIndex;

		size_t numComps = userInterface->getNumComponents();

		if(startIndex < 0)							startIndex = 0;
		if(endIndex > numComps || endIndex < 0)		endIndex = numComps;
		if(endIndex < startIndex)					return;

		userInterface->calcComponentsScreenCoords();

		if(uiShaderGroup.renderObjects.size() == 0)
			return;
		else if(uiShaderGroup.renderObjects[0].VBOs.size() == 0)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, uiShaderGroup.renderObjects[0].VBOs[0]);

		///GLfloat * buffer = (GLfloat *)glMapBufferRange(GL_ARRAY_BUFFER, startIndex * 16 * sizeof(GLfloat), (endIndex-startIndex) * 16 * sizeof(GLfloat), GL_MAP_WRITE_BIT);
		GLfloat * buffer = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		for(size_t i = startIndex, j = 0; i < endIndex; i++)
		{
			osi::UIComponent * comp = userInterface->getComponent(i);

			///coords.x = x, coords.y = y, coords.z = width, coords.w = height
			///std::vector<glm::vec4> coords = comp->getComponentScreenCoords(0, 0, userInterface->getFramebufferWidth(), userInterface->getFramebufferHeight());
			glm::vec4 coords = comp->getComponentScreenCoords();

			int t = comp->getCurrentTex() < userInterface->texCoords.size() ? comp->getCurrentTex() : -1;	//Determine which set of texture co-ordinates to use

			if(t != -1)
			{
				///std::cerr << "START INDEX: " << startIndex << ", END INDEX: " << endIndex << "\n";
				///std::cerr << "SIZEOF BUFFER: " << sizeof(buffer) << ", J: " << j << "\n";

				//Two position coordinates followed by two texture co-ordinates
				buffer[j * 16]      = coords.x;
				buffer[j * 16 + 1]  = coords.y;

				buffer[j * 16 + 4]  = coords.x + coords.z;
				buffer[j * 16 + 5]  = coords.y;

				buffer[j * 16 + 8]  = coords.x + coords.z;
				buffer[j * 16 + 9]  = coords.y + coords.w;

				buffer[j * 16 + 12] = coords.x;
				buffer[j * 16 + 13] = coords.y + coords.w;

				j++;	//Only increase position in buffer if texture is present
			}
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		fullUpdateUserInterfaceText(userInterface);
	}

	void RenderingEngine::fullUpdateUserInterface(osi::UserInterface * userInterface)	//Update user interface shaders on resize of framebuffer
	{
		shouldUpdateUI = false;

		userInterface->calcComponentsScreenCoords();

		if(uiShaderGroup.renderObjects.size() == 0)
			return;
		else if(uiShaderGroup.renderObjects[0].VBOs.size() == 0)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, uiShaderGroup.renderObjects[0].VBOs[0]);

		GLint vboSize;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vboSize);

		size_t numComps =  userInterface->getNumComponents();
		
		if(vboSize < numComps * 16 * sizeof(GLfloat))
		{
			///std::cerr << "Resizing UI Buffer, From: " << vboSize << ", To: " << numComps * 16 * sizeof(GLfloat) << std::endl;
			glBufferData(GL_ARRAY_BUFFER, numComps * 16 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
		}


		GLfloat * buffer = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		for(size_t i = 0, j = 0; i < userInterface->getNumComponents(); i++)	//WARNING - Do not replace getNumComponents() with numComps
		{
			osi::UIComponent * comp = userInterface->getComponent(i);

			///std::vector<glm::vec4> coords = comp->getComponentScreenCoords(0, 0, userInterface->getFramebufferWidth(), userInterface->getFramebufferHeight());
			glm::vec4 coords = comp->getComponentScreenCoords();

			//Two position coordinates followed by two texture co-ordinates

			os_int_t t = comp->getCurrentTex() < userInterface->texCoords.size() ? comp->getCurrentTex() : -1;	//Determine which set of texture co-ordinates to use
			
			if(t != -1)
			{
				buffer[j * 16]      = coords.x;
				buffer[j * 16 + 1]  = coords.y;
				buffer[j * 16 + 2]  = userInterface->texCoords[t][0];	//0, 1
				buffer[j * 16 + 3]  = userInterface->texCoords[t][3];

				buffer[j * 16 + 4]  = coords.x + coords.z;
				buffer[j * 16 + 5]  = coords.y;
				buffer[j * 16 + 6]  = userInterface->texCoords[t][1];	//1, 1
				buffer[j * 16 + 7]  = userInterface->texCoords[t][3];

				buffer[j * 16 + 8]  = coords.x + coords.z;
				buffer[j * 16 + 9]  = coords.y + coords.w;
				buffer[j * 16 + 10] = userInterface->texCoords[t][1];	//1, 0
				buffer[j * 16 + 11] = userInterface->texCoords[t][2];

				buffer[j * 16 + 12] = coords.x;
				buffer[j * 16 + 13] = coords.y + coords.w;
				buffer[j * 16 + 14] = userInterface->texCoords[t][0];	//0, 0
				buffer[j * 16 + 15] = userInterface->texCoords[t][2];

				j++;	//Only increase the position in the buffer if a texture is present
			}
			else
			{
				numComps--;	//Component does not have a texture therefore does not need to be displayed
			}
		}

		uiShaderGroup.renderObjects[0].numVerticesPerRenderCall[0] = 4 * numComps;

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//Only updates text if called immediately after updateUserInterface or fullUpdateUserInterface
	void RenderingEngine::fullUpdateUserInterfaceText(osi::UserInterface * ui)
	{
		if(textShaderGroup.renderObjects.size() == 0)
			return;
		else if(textShaderGroup.renderObjects[0].VBOs.size() == 0)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, textShaderGroup.renderObjects[0].VBOs[0]);

		os_int_t vboSize;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vboSize);

		int numChars =  ui->getNumTextChars();
		textShaderGroup.renderObjects[0].numVerticesPerRenderCall[0] = 4 * numChars;

		if(vboSize < numChars * 16 * sizeof(os_float_t))
		{
			///std::cerr << "Resizing UI Buffer, From: " << vboSize << ", To: " << numChars * 16 * sizeof(GLfloat) << std::endl;
			glBufferData(GL_ARRAY_BUFFER, numChars * 16 * sizeof(os_float_t), NULL, GL_STATIC_DRAW);
		}


		os_float_t * buffer = (os_float_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		for(size_t i = 0, j = 0, n = 0; i < ui->getComponents().size(); i++)
		{
			osi::UIComponent * comp = ui->getComponent(i);

			std::vector<glm::vec4> coords {comp->getComponentScreenCoords()};

			for(size_t k = 0; k < coords.size(); k++, j++)	//coords cotains coordinates of components and all sub-components
			{
				osi::Origin orig = comp->getTextOrigin();
				const std::string & text = comp->getText();

				if(text == "") continue;

				osi::Font & font = ui->getFontManager()->getFont(0);

				os_float_t scale = 0.28f;
				os_float_t sumWidths = 0, maxHeight = 0;
				size_t numChars = 0;
				for(std::string::const_iterator c = text.begin(); c != text.end(); ++c)
				{
					osi::Character & ch = font.chars[*c];

					os_float_t width = (os_float_t)(ch.size.x + ch.bearing.x) * scale;
					if(sumWidths + width * 2 > coords[k].z)	//If the character would be outside the component, then break so num chars is capped and any subsequent chars are not displayed
						break;
					numChars++;
					sumWidths += width;
					maxHeight = std::max(maxHeight * scale, (os_float_t)(ch.size.y) * scale);
				}

				os_float_t x = 0.0f, y = 0.0f;
				switch(orig)	//TODO - Add remainder of origins
				{
				case osi::Origin::LEFT_CENTREY:
					///std::cerr << "1\n";
					x = coords[k].x - font.chars[text.at(0)].bearing.x * scale;
					y = coords[k].y + coords[k].w/2 - scale*font.chars['a'].size.y/2;
					break;
				case osi::Origin::CENTREX_CENTREY:
					///std::cerr << "0\n";
					x = coords[k].x + coords[k].z/2 - sumWidths/2 - font.chars[text.at(0)].bearing.x * scale;
					y = coords[k].y + coords[k].w/2 - scale*font.chars['a'].size.y/2;
					break;
				default:
					///std::cerr << "DEFAULT\n";
					x = coords[k].x + coords[k].z/2 - sumWidths/2 - font.chars[text.at(0)].bearing.x * scale;
					y = coords[k].y + coords[k].w/2 - scale*font.chars['a'].size.y/2;
					break;
				}

				//Iterate through all characters
				std::string::const_iterator c;
				size_t i = 0;
				for(c = text.begin(); c != text.end() && i < numChars; ++c, i++)
				{
					osi::Character & ch = font.chars[*c];

					os_float_t xpos = x + (os_float_t)(ch.bearing.x * scale);
					os_float_t ypos = y - (os_float_t)((ch.size.y - ch.bearing.y) * scale);

					os_float_t w = (os_float_t)ch.size.x * scale;
					os_float_t h = (os_float_t)ch.size.y * scale;

					///std::cerr << "CHAR: " << (*c) << std::endl;
					///std::cerr << "x = " << xpos << ", y = " << ypos << ", w = " << w << ", h = " << h << std::endl;
					///std::cerr << "Tex: " << font.texCoords[(*c)*2][0] << ", " << font.texCoords[(*c)*2][1] << std::endl << std::endl;

					//Update VBO for each character
					buffer[n * 16]       = xpos;
					buffer[n * 16 + 1]   = ypos;
					buffer[n * 16 + 2]   = font.texCoords[(*c)][0];
					buffer[n * 16 + 3]   = font.texCoords[(*c)][3];	//0, 1

					buffer[n * 16 + 4]   = xpos + w;
					buffer[n * 16 + 5]   = ypos;
					buffer[n * 16 + 6]   = font.texCoords[(*c)][1];
					buffer[n * 16 + 7]   = font.texCoords[(*c)][3];	//1, 1

					buffer[n * 16 + 8]   = xpos + w;
					buffer[n * 16 + 9]   = ypos + h;
					buffer[n * 16 + 10]  = font.texCoords[(*c)][1];
					buffer[n * 16 + 11]  = font.texCoords[(*c)][2];	//1, 0

					buffer[n * 16 + 12]  = xpos;
					buffer[n * 16 + 13]  = ypos + h;
					buffer[n * 16 + 14]  = font.texCoords[(*c)][0];
					buffer[n * 16 + 15]  = font.texCoords[(*c)][2];	//0, 0

					n++;	//Increment total character count

							//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					x += (ch.advance.x >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
					///y += (ch.advance.y >> 6);
				}
			}
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}



	void RenderingEngine::updateUBOLighting()
	{
		shouldUpdateUBOLighting = false;

		glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
		GLfloat * uboLightingData = (GLfloat*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

		int numPointLights = (int)(*pointLights).size();
		if(numPointLights > 16)
			numPointLights = 16;	//Max num point lights = 16
		if(numPointLights > 0)
			memcpy(uboLightingData, &(*pointLights)[0], sizeof(osr::PointLight) * numPointLights);

		int numDirLights = (int)(*dirLights).size();
		if(numDirLights > 1)
			numDirLights = 1;	//Max num direction lights = 1
		if(numDirLights > 0)
			memcpy(uboLightingData + 16*20, &(*dirLights)[0], sizeof(osr::DirectionLight) * numDirLights);

		int numSpotLights = (int)(*spotLights).size();
		if(numSpotLights > 16)
			numSpotLights = 16;	//Max num spot lights = 16
		if(numSpotLights > 0)
			memcpy(uboLightingData + 16*20 + 1*20, &(*spotLights)[0], sizeof(osr::SpotLight) * numSpotLights);

		//Update the number of light in the shader
		glUseProgram(framebufferProg);
		glUniform1i(glGetUniformLocation(framebufferProg, "numPointLights"), numPointLights);
		glUniform1i(glGetUniformLocation(framebufferProg, "numDirLights"), numDirLights);
		glUniform1i(glGetUniformLocation(framebufferProg, "numSpotLights"), numSpotLights);
		glUseProgram(0);

		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}



	//func is a bitfield to reduce function search time
	/*
		Basic Type:   0 - f, 1 - i, 2 - u, 3 - d
		Complex Type: 0 - s, 1 - v, 2 - m
		Amount:		  0 - 1, 1 - 2, 2 - 3, 3 - 4
	*/
	//Data array should be an array of singles, a pointer to a vector or a pointer to a matrix
	void RenderingEngine::uniform(os_uint_t func, os_uint_t loc, void * dataArray) const
	{
		os_uint_t basicType =	(func & 0b11);
		os_uint_t complexType = (func & 0b1100) >> 2;
		os_uint_t amount	  = (func & 0b110000) >> 4;

		switch(complexType)
		{
		case 0:					//Singles
			switch(basicType)
			{
			case 0:				//Floats
				{
					os_float_t * arr = (os_float_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 Float
						glUniform1f(loc, arr[0]);
						break;
					case 1:			//2	Floats
						glUniform2f(loc, arr[0], arr[1]);
						break;
					case 2:			//3	Floats
						glUniform3f(loc, arr[0], arr[1], arr[2]);
						break;
					case 3:			//4	Floats
						glUniform4f(loc, arr[0], arr[1], arr[2], arr[3]);
						break;
					}
				}
				break;
			case 1:				//Ints
				{
					os_int_t * arr = (os_int_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 Int
						glUniform1i(loc, arr[0]);
						break;
					case 1:			//2	Int
						glUniform2i(loc, arr[0], arr[1]);
						break;
					case 2:			//3	Int
						glUniform3i(loc, arr[0], arr[1], arr[2]);
						break;
					case 3:			//4	Int
						glUniform4i(loc, arr[0], arr[1], arr[2], arr[3]);
						break;
					}
				}
				break;
			case 2:				//U-Ints
				{
					os_uint_t * arr = (os_uint_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 U-Int
						glUniform1ui(loc, arr[0]);
						break;
					case 1:			//2	U-Int
						glUniform2ui(loc, arr[0], arr[1]);
						break;
					case 2:			//3	U-Int
						glUniform3ui(loc, arr[0], arr[1], arr[2]);
						break;
					case 3:			//4	U-Int
						glUniform4ui(loc, arr[0], arr[1], arr[2], arr[3]);
						break;
					}
				}
				break;
			case 3:				//Doubles
				{
					os_double_t * arr = (os_double_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 U-Int
						glUniform1d(loc, arr[0]);
						break;
					case 1:			//2	U-Int
						glUniform2d(loc, arr[0], arr[1]);
						break;
					case 2:			//3	U-Int
						glUniform3d(loc, arr[0], arr[1], arr[2]);
						break;
					case 3:			//4	U-Int
						glUniform4d(loc, arr[0], arr[1], arr[2], arr[3]);
						break;
					}
				}
				break;
			}
			break;
		case 1:					//Vectors
			switch(basicType)
			{
			case 0:				//Floats
				{
					os_float_t * vec = (os_float_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 Float
						glUniform1fv(loc, 1, vec);
						break;
					case 1:			//2	Floats
						glUniform2fv(loc, 1, vec);
						break;
					case 2:			//3	Floats
						glUniform3fv(loc, 1, vec);
						break;
					case 3:			//4	Floats
						glUniform4fv(loc, 1, vec);
						break;
					}
				}
				break;
			case 1:				//Ints
				{
					os_int_t * vec = (os_int_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 Int
						glUniform1iv(loc, 1, vec);
						break;
					case 1:			//2	Ints
						glUniform2iv(loc, 1, vec);
						break;
					case 2:			//3	Ints
						glUniform3iv(loc, 1, vec);
						break;
					case 3:			//4	Ints
						glUniform4iv(loc, 1, vec);
						break;
					}
				}
				break;
			case 2:				//U-Ints
				{
					os_uint_t * vec = (os_uint_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 UInt
						glUniform1uiv(loc, 1, vec);
						break;
					case 1:			//2	UInts
						glUniform2uiv(loc, 1, vec);
						break;
					case 2:			//3	UInts
						glUniform3uiv(loc, 1, vec);
						break;
					case 3:			//4	UInts
						glUniform4uiv(loc, 1, vec);
						break;
					}
				}
				break;
			case 3:				//Doubles
				{
					os_double_t * vec = (os_double_t *)(dataArray);
					switch(amount)
					{
					case 0:			//1 Doubles
						glUniform1dv(loc, 1, vec);
						break;
					case 1:			//2	Doubles
						glUniform2dv(loc, 1, vec);
						break;
					case 2:			//3	Doubles
						glUniform3dv(loc, 1, vec);
						break;
					case 3:			//4	Doubles
						glUniform4dv(loc, 1, vec);
						break;
					}
				}
				break;
			}
			break;
		case 2:					//Matrices
			os_uint_t amount2   = (func & 0b11000000) >> 6;
			switch(basicType)
			{
			case 0:				//Floats
				{
					os_float_t * mat = (os_float_t *)(dataArray);
					switch(amount)	//TODO - Use two remaining bits to allow non-square matrices
					{
					case 1:
						{
							switch(amount2)
							{
							case 0:
								{
									glUniformMatrix2fv(loc, 1, GL_FALSE, mat);
								}
							case 1:
								{
									glUniformMatrix2x3fv(loc, 1, GL_FALSE, mat);
								}
							case 2:
								{
									glUniformMatrix2x4fv(loc, 1, GL_FALSE, mat);
								}
							}
						}
						break;
					case 2:
						{
							switch(amount2)
							{
							case 0:
								{
									glUniformMatrix3x2fv(loc, 1, GL_FALSE, mat);
								}
							case 1:
								{
									glUniformMatrix3fv(loc, 1, GL_FALSE, mat);
								}
							case 2:
								{
									glUniformMatrix3x4fv(loc, 1, GL_FALSE, mat);
								}
							}
						}
						break;
					case 3:
						{
							switch(amount2)
							{
							case 0:
								{
									glUniformMatrix4x2fv(loc, 1, GL_FALSE, mat);
								}
							case 1:
								{
									glUniformMatrix4x3fv(loc, 1, GL_FALSE, mat);
								}
							case 2:
								{
									glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
								}
							}
						}
						break;
					}
				}
				break;
			default:
				{
					std::cerr << "ERROR: Incompatible type: " << basicType << " with type Matrix" << std::endl;
					break;
				}
			}
			break;
		}
	}










	/**
	 * Setup functions
	 * */
	/**Sets up depth buffer with z-near-z-far of 0-1 and GL_LEQUAL and enables depth testing*/
	void RenderingEngine::initDepthTesting()
	{
		glEnable(GL_DEPTH_TEST);	//Enable depth test
		glDepthMask(GL_TRUE);		//Set depth buffer enabled for writing
		glDepthFunc(GL_LEQUAL);		//New depth value written to buffer if it is less than or equal to current one
		glDepthRange(0.0f, 1.0f);	//Set window space z-near and z-far
	}

	/**Enables back face culling with clockwise winding as the front face*/
	void RenderingEngine::initFaceCulling()
	{
		glEnable(GL_CULL_FACE);	//Enable face culling
		glCullFace(GL_BACK);	//Set the face to be culled as the back face
		glFrontFace(GL_CCW);	//Set the front face to be wound clockwise
	}





	/**Calculates the frustum scale for a given field of view angle in degrees*/
	/*float RenderingEngine::calcFrustumScale(const float fovDeg) const
	{
		const float degToRad = 3.14159f * 2.0f / 360.0f;	//Calculate constant to convert degrees into radians
		float fovRad = degToRad * fovDeg;					//Convert the fov from degrees into radians
		return 1.0f / tan(fovRad / 2.0f);					//Calculates and returns the frustum scale
	}*/

	glm::mat4 RenderingEngine::genPerspectiveMatrix(const os_float_t fovRadians, const os_int_t width, const os_int_t height)	//Generates and returns a 16x16 perspective projection matrix
	{
		//float frustumScale = calcFrustumScale(fovDeg);
		float zNear = 0.5f, zFar = 250.0f;

		/*glm::mat4 mat;

		mat[0].x = frustumScale;
		mat[1].y = frustumScale;
		mat[2].z = (zFar + zNear) / (zNear - zFar);
		mat[2].w = -1.0f;
		mat[3].z = (2 * zFar * zNear) / (zNear - zFar);*/
		
		glm::mat4 mat = glm::perspective(fovRadians, (16/9)*((os_float_t)width/(os_float_t)height), zNear, zFar);
		//glm::mat4 mat = glm::ortho(-width/2, width/2, -height/2, height/2)

		return mat;
	}

	void RenderingEngine::setFOV(os_float_t angleDegrees)	//Sets the fov of the perspective projection matrix
	{
		fovRadians = glm::radians(angleDegrees);
		perspectiveProjection = genPerspectiveMatrix(fovRadians, framebufferWidth, framebufferHeight);

		//Update the shaders with the new perspective projection matrix
		os_uint_t blockIndexMatrices = 0;						//block index ~= uniform location
		os_uint_t bindingPointMatrices = 0;
		glBindBuffer(GL_UNIFORM_BUFFER, uboIDMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(perspectiveProjection));	//First mat4 so offest = 0 and size = sizeof(mat4)
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointMatrices, uboIDMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void RenderingEngine::windowPosCallbackImpl(os_int_t x, os_int_t y)
	{
		/*framebufferX = x;
		y = framebufferY = framebufferY - (framebufferY - y);
		orthographicProjection = glm::ortho(0.0f, (GLfloat)framebufferWidth, 0.0f, (GLfloat)framebufferHeight);

		glViewport(0, 0, (GLfloat)framebufferWidth, (GLfloat)framebufferHeight);

		glUseProgram(uiShaderGroup.shaderPrograms[0].program);
		glUniformMatrix4fv(glGetUniformLocation(uiShaderGroup.shaderPrograms[0].program, "orthoProjection"), 1, GL_FALSE, glm::value_ptr(orthographicProjection));
		glUseProgram(0);*/
	}
	
	void RenderingEngine::framebufferSizeCallbackImpl(os_int_t width, os_int_t height)
	{
		framebufferWidth = width;
		framebufferHeight = height;
		perspectiveProjection = genPerspectiveMatrix(fovRadians, width, height);
		orthographicProjection = glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height);

		glViewport(0, 0, (GLsizei)width, (GLsizei)height);
		updateGBufferFBO(width, height);

		for(size_t rp = 0; rp < renderPasses.size(); rp++)
		{
			for(size_t i = 0; i < renderPasses[rp].shaderGroups.size(); i++)
			{
				ShaderGroup & shaderGroup = renderPasses[rp].shaderGroups[i];
				for(size_t j = 0; j < shaderGroup.shaderPrograms.size(); j++)
				{
					glUseProgram(shaderGroup.shaderPrograms[j].program);
					glUniformMatrix4fv(glGetUniformLocation(shaderGroup.shaderPrograms[j].program, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(perspectiveProjection));
				}
			}
		}
		if(skyboxShaderGroup.shaderPrograms.size() > 0)
		{
			glUseProgram(skyboxShaderGroup.shaderPrograms[0].program);
			glUniformMatrix4fv(glGetUniformLocation(skyboxShaderGroup.shaderPrograms[0].program, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(perspectiveProjection));
		}
		glUseProgram(0);

		//Update the shaders with the new perspective projection matrix
		os_uint_t blockIndexMatrices = 0;						//block index ~= uniform location
		os_uint_t bindingPointMatrices = 0;
		glBindBuffer(GL_UNIFORM_BUFFER, uboIDMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(perspectiveProjection));	//First mat4 so offest = 0 and size = sizeof(mat4)
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointMatrices, uboIDMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		if(uiShaderGroup.shaderPrograms.size() > 0)
		{
			glUseProgram(uiShaderGroup.shaderPrograms[0].program);
			glUniformMatrix4fv(glGetUniformLocation(uiShaderGroup.shaderPrograms[0].program, "orthoProjection"), 1, GL_FALSE, glm::value_ptr(orthographicProjection));
		}
		if(textShaderGroup.shaderPrograms.size() > 0)
		{
			glUseProgram(textShaderGroup.shaderPrograms[0].program);
			glUniformMatrix4fv(glGetUniformLocation(textShaderGroup.shaderPrograms[0].program, "orthoProjection"), 1, GL_FALSE, glm::value_ptr(orthographicProjection));
		}
		glUseProgram(0);
	}

	os_uint_t RenderingEngine::updateGBufferFBO(os_int_t width, os_int_t height)
	{
		glBindTexture(GL_TEXTURE_2D, gPos);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		
		glBindTexture(GL_TEXTURE_2D, gColourSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		
		updateGDepthRBO(width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gDepthRBO);

		return 0;
	}

	os_uint_t RenderingEngine::updateGDepthRBO(os_int_t width, os_int_t height)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, gDepthRBO);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		return 0;
	}















	//Generate a vertex buffer object from the data array and return an ID of it
	os_uint_t RenderingEngine::genVBO(const void * data, const os_sizeiptr_t sizeBytes, const GLenum hint) const
	{
		os_uint_t vboID;
		glGenBuffers(1, &vboID);

		glBindBuffer(GL_ARRAY_BUFFER, vboID);

		glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, hint);	//Put the data into the vbo

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return vboID;
	}

	//Creates an index buffer object using unsigned shorts and returns the IBO ID
	os_uint_t RenderingEngine::genIBO(const os_ushort_t * data, const os_sizeiptr_t sizeArray, const GLenum hint) const
	{
		os_uint_t iboID;
		glGenBuffers(1, &iboID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

		//Note - Uses unsigned short for element buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeArray * sizeof(GLushort), data, hint);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return iboID;
	}

	//Creates an index buffer object using unsigned ints and returns the IBO ID
	os_uint_t RenderingEngine::genIBOINT(const os_uint_t * data, const os_sizeiptr_t sizeArray, const GLenum hint) const
	{
		os_uint_t iboID;
		glGenBuffers(1, &iboID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

		//Note - Uses unsigned short for element buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeArray * sizeof(os_uint_t), data, hint);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return iboID;
	}

	os_uint_t RenderingEngine::genVAO(const std::vector<VertexAttrib> & vertexAttribs, os_uint_t IBO) const	//Creates a vertex array object
	{
		os_uint_t vaoID;
		glGenVertexArrays(1, &vaoID);

		glBindVertexArray(vaoID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		os_uint_t prevVBO = 0;

		for(size_t i = 0; i < vertexAttribs.size(); i++)
		{
			const VertexAttrib & attrib = vertexAttribs[i];

			if(attrib.VBO != prevVBO)
			{
				prevVBO = attrib.VBO;
				glBindBuffer(GL_ARRAY_BUFFER, attrib.VBO);
			}

			glEnableVertexAttribArray(attrib.location);
			if(attrib.type == GL_BYTE		|| attrib.type == GL_UNSIGNED_BYTE
				|| attrib.type == GL_SHORT	|| attrib.type == GL_UNSIGNED_SHORT
				|| attrib.type == GL_INT	|| attrib.type == GL_UNSIGNED_INT)
			{
				glVertexAttribIPointer(attrib.location, attrib.size, attrib.type, attrib.stride, (GLvoid *)attrib.offset);
			} else if(attrib.type == GL_DOUBLE)
			{
				glVertexAttribLPointer(attrib.location, attrib.size, attrib.type, attrib.stride, (GLvoid *)attrib.offset);
			} else
			{
				glVertexAttribPointer(attrib.location, attrib.size, attrib.type, attrib.normalised, attrib.stride, (GLvoid *)attrib.offset);
			}

			if(attrib.divisor != 4)
			{
				if(GLEW_VERSION_3_2)
					//std::cerr << "OpenGL 3.2 Available: Using glVertexAttribDivisor" << std::endl;
					glVertexAttribDivisor(attrib.location, attrib.divisor);
				else if(GLEW_ARB_instanced_arrays)
					//std::cerr << "OpenGL 3.2 Un-Available, ARB_istanced_arrays Available: Using glVertexAttribDivisorARB" << std::endl;
					glVertexAttribDivisorARB(attrib.location, attrib.divisor);
				else
					std::cerr << "ERROR: Vertex Attrib Divisor is Unavailable" << std::endl;
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return vaoID;
	}

	os_uint_t RenderingEngine::genUBO(os_sizeiptr_t sizeBytes) const	//Creates a uniform buffer object capable of storing the given size (in bytes)
	{
		os_uint_t uboID;
		glGenBuffers(1, &uboID);

		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		glBufferData(GL_UNIFORM_BUFFER, sizeBytes, NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return uboID;
	}

	void RenderingEngine::genMaterialUBO(std::vector<Material> materials)	//Creates a uniform buffer object specifically for materials
	{
		int uniformBufferAlignSize = 0;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

		unsigned sizeMaterialBlock = sizeof(Material);
		sizeMaterialBlock += uniformBufferAlignSize - (sizeMaterialBlock % uniformBufferAlignSize);

		int sizeMaterialUniformBuffer = sizeMaterialBlock * materials.size();

		std::vector<GLubyte> mtlBuffer;
		mtlBuffer.resize(sizeMaterialUniformBuffer, 0);

		GLubyte * bufferPtr = &mtlBuffer[0];

		for(size_t mtl = 0; mtl < materials.size(); ++mtl)
		{
			memcpy(bufferPtr + (mtl * sizeMaterialBlock), &materials[mtl], sizeof(Material));
		}

		glGenBuffers(1, &matUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, matUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeMaterialUniformBuffer, bufferPtr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}










	//Creates a shader of the specified type by compiling the shader file passed
	os_uint_t RenderingEngine::createShader(GLenum eShaderType, const std::string & strShaderCode) const
	{
		os_uint_t shader = glCreateShader(eShaderType);        	//Create a new shader

		const char * ptrShaderSource = strShaderCode.c_str();

		glShaderSource(shader, 1, &ptrShaderSource, NULL);      //Sets the source file of the shader
		glCompileShader(shader);    //Compile the shader source

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);  //Get the compile status of the shader
		if(status == GL_FALSE)                              //If the shader source failed to compile
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);  //Get the length of the info log

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];         ///Create a C string to store the info log
			glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);//Get the info log and store it in the C string

			const char *strShaderType = NULL;
			switch(eShaderType)                     //Get the type of shader as a string from the shader type GLenum
			{
				case GL_VERTEX_SHADER:      strShaderType = "vertex";    break;
				case GL_GEOMETRY_SHADER:    strShaderType = "geometry";  break;
				case GL_FRAGMENT_SHADER:    strShaderType = "fragment";  break;
			}

			fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);  //Print an error message and the info log
			delete[] strInfoLog;                                                                //Free up resources by deleting the info log string
		}

		return os_uint_t(shader);
	}

	//Creates a shader program by linking the passed shaders together
	os_uint_t RenderingEngine::createShaderProgram(const std::vector<os_uint_t> & shaders) const
	{
		os_uint_t program = glCreateProgram();     //Create a shader program

		for(size_t iLoop = 0; iLoop < shaders.size(); iLoop++)  //For each shader in shaders list
		{
			glAttachShader(program, shaders[iLoop]);            //Attach the shader to the shader program
		}

		glLinkProgram(program);     //Link the shader program

		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);   //Get the link status of the shader program
		if(status == GL_FALSE)                              //If the link failed
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);    //Get the length of the info log from the program

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];             //Create a C string to store the info log
			glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);  //Get and store the info log in the info log string
			fprintf(stderr, "Linker failure: %s\n", strInfoLog);            //Print an error message with the info log
			delete[] strInfoLog;                                            //Free up resources by deleting the info log string
		}

		return program;         //Return the GLint ID of the shader program
	}

	//Called for each shader after shader programs have been created
	void RenderingEngine::deleteShader(const os_uint_t shader) const
	{
		glDeleteShader(shader);   //Delete the shader
	}




	os_uint_t RenderingEngine::createTexture(const os_ubyte_t * img, const os_int_t width, const os_int_t height, os_bool_t loadAlpha) const
	{
		os_uint_t texID;
		glGenTextures(1, &texID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		if(loadAlpha)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		if(useMIPMapping)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		//TODO - Replace glTexParameter with glSamplerParameter to improve perfomance
		if(texFilterMode == TexFilterMode::NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else if(texFilterMode == TexFilterMode::LINEAR)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if(texFilterMode == TexFilterMode::LINEAR_MIP_MAP_NEAREST)	//Assumes MIP maps are enabled
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if(texFilterMode == TexFilterMode::LINEAR_MIP_MAP_LINEAR)		//Assumes MIP maps are enabled
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if(texFilterMode == TexFilterMode::ANISOTROPIC_MIP_MAP_LINEAR)//Assumes MIP maps are enabled
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisotropicLevel);

		}

		glBindTexture(GL_TEXTURE_2D, 0);

		return texID;
	}

	//Tex Coords will be cleared and filled with absolute texture co-ordinates
	os_uint_t RenderingEngine::createTextureAtlas(std::vector<os_ubyte_t *> imgs, std::vector<os_int_t> widths, std::vector<os_int_t> heights,
		std::vector<glm::vec4> & texCoords, os_bool_t useEqualDimensions, os_int_t internalFormat, GLenum format)
	{
		//std::cerr << "ERROR: " << glGetError() << std::endl;

		if(imgs.size() != widths.size() || widths.size() != heights.size())	//Check there are widths and heights for each image
		{
			std::cerr << "Create Texture Atlas Failed: widths.size != heights.size != imgs.size, Returing 0 from function" << std::endl;
			return 0;
		}

		if(texCoords.size() > 0)
			texCoords.clear();

		os_uint_t texID;
		glGenTextures(1, &texID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//1 byte per pixel
		
		os_int_t totalWidth = 0, totalHeight = 0;
		for(size_t i = 0; i < widths.size(); i++)				//Calculate the width and height for a row of imgs
		{
			totalWidth += widths[i];							//Calculate the width of the texture atlas
			totalHeight = std::max(totalHeight, heights[i]);	//Calculate the height of the texture atlas
		}

		GLint maxTexSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
		//std::cerr << "TOTAL WIDTH: " << totalWidth << "\n";
		//std::cerr << "MAX TEX WIDTH: " << maxTexSize << "\n";

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, totalWidth, totalHeight, 0, format, GL_UNSIGNED_BYTE, 0);	//Create a texture of the correct size

		int x = 0;
		for(size_t i = 0; i < imgs.size(); i++)		//Store images sequentially in a row
		{
			//Calculate texture co-ordinates
			//Half pixel correction used (x +/- 0.5f) to prevent texture bleeding
			glm::vec4 uvTexCoords;										//Relative u, v coords:
			uvTexCoords[0] = ((GLfloat)x + 0.5f)				/(GLfloat)totalWidth;	//x : 0
			uvTexCoords[1] = ((GLfloat)(x + widths[i]) - 0.5f)	/(GLfloat)totalWidth;	//x : 1
			uvTexCoords[2] = 0.0f;														//y : 0
			uvTexCoords[3] = ((GLfloat)(heights[i]) - 0.5f)		/(GLfloat)totalHeight;	//y : 1
			
			texCoords.push_back(uvTexCoords);	//Push this textures tex coords into tex coord vector

			//Append texture onto texture atlas
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, widths[i], heights[i], format, GL_UNSIGNED_BYTE, imgs[i]);
			x += widths[i];
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		//std::cerr << "ERROR: " << glGetError() << std::endl;

		return texID;
	}


	os_uint_t RenderingEngine::createCubemapTexture(std::vector<os_ubyte_t *> imgs,
			std::vector<os_int_t> widths, std::vector<os_int_t> heights) const
	{
		if(imgs.size() != widths.size() || imgs.size() != heights.size())
		{
			std::cerr << "At createCubemapTexture, imgs.size != dimensions.size, aborting function call!" << std::endl;
			return 0;
		}

		os_uint_t textureID;
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for(os_uint_t i = 0; (i < imgs.size()) && (i < 6); i++)	//Will create textures for up to 6 faces
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, widths[i], heights[i], 0, GL_RGB, GL_UNSIGNED_BYTE, imgs[i]);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);		//Prevents flickering
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return textureID;
	}












	/**
	 * Change of state functions
	 * */
	/**Enable or disable depth testing*/
	void RenderingEngine::setDepthTesting(os_bool_t enabled)
	{
		if(enabled)	glEnable(GL_DEPTH_TEST);
		else		glDisable(GL_DEPTH_TEST);
	}

	/**Enable or disable face culling*/
	void RenderingEngine::setFaceCulling(os_bool_t enabled)
	{
		if(enabled)	glEnable(GL_CULL_FACE);
		else		glDisable(GL_CULL_FACE);
	}

	/**Enable or disable depth clamping*/
	void RenderingEngine::setDepthClamping(os_bool_t enabled)
	{
		//Enabling depth clamping disables clipping and fragments outside clip range are clamped into range
		if(enabled)	glEnable(GL_DEPTH_CLAMP);
		else		glDisable(GL_DEPTH_CLAMP);
	}



	void RenderingEngine::setUseVsync(os_bool_t enabled)
	{
		useVsync = enabled;
		if(enabled)		glfwSwapInterval(1);
		else			glfwSwapInterval(0);
	}

	void RenderingEngine::setUseMIPMapping(os_bool_t enabled)
	{
		useMIPMapping = enabled;
	}

	void RenderingEngine::setUseTripleBuffering(os_bool_t enabled)
	{
		useTripleBuffering = enabled;
	}

	void RenderingEngine::setAntiAliasingMode(AAMode mode)
	{
		antiAliasingMode = mode;
	}

	void RenderingEngine::setTextureFilteringMode(TexFilterMode mode)
	{
		texFilterMode = mode;
	}

	void RenderingEngine::setAnisotropicLevel(GLfloat level)
	{
		anisotropicLevel = level;
	}








	/*void RenderingEngine::renderText(osi::FontManager * fontManager, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
	{
		//Activate corresponding render state	
		glUseProgram(textShaderGroup.shaderPrograms[0].program);
		glUniform3f(glGetUniformLocation(textShaderGroup.shaderPrograms[0].program, "textColour"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, textShaderGroup.renderObjects[0].VBOs[0]);
		glBindVertexArray(textShaderGroup.renderObjects[0].VAO);

		//Iterate through all characters
		std::string::const_iterator c;
		for(c = text.begin(); c != text.end(); c++)
		{
			osi::Character & ch = fontManager->getFont(0).chars[*c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;
			//Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			//Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.texID);
			//Update content of VBO memory
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
			//Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.advance >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}*/










	/*void RenderingEngine::setUseVsync(bool enabled)
	{
		useVsync = enabled;
		if(useVsync)	glfwSwapInterval(1);
		else			glfwSwapInterval(0);
	}

	void RenderingEngine::setUseMIPMapping(bool enabled)	//TODO - Implement MIP mapping
	{

	}

	void RenderingEngine::setUseTripleBuffering(bool enabled)	//TODO - Implement or remove this function
	{

	}

	void RenderingEngine::setAntiAliasingMode(AAMode mode)		//TODO - Implement AA modes
	{
		antiAliasingMode = mode;
	}

	void RenderingEngine::setBrightness(float val)
	{
		brightness = val;
	}

	void RenderingEngine::setTextureQuality(Quality q)
	{
		textureQuality = q;
	}

	void RenderingEngine::setMeshQuality(Quality q)
	{
		meshQuality = q;
	}*/
}
