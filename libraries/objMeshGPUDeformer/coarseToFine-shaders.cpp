/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 2.1                               *
 *                                                                       *
 * "objMeshGPUDeformer" library , Copyright (C) 2007 CMU, 2009 MIT,      *
 *                                                        2014 USC       *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code author: Jernej Barbic                                            *
 * http://www.jernejbarbic.com/code                                      *
 *                                                                       *
 * Research: Jernej Barbic, Fun Shing Sin, Daniel Schroeder,             *
 *           Doug L. James, Jovan Popovic                                *
 *                                                                       *
 * Funding: National Science Foundation, Link Foundation,                *
 *          Singapore-MIT GAMBIT Game Lab,                               *
 *          Zumberge Research and Innovation Fund at USC                 *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the BSD-style license that is            *
 * included with this library in the file LICENSE.txt                    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the file     *
 * LICENSE.TXT for more details.                                         *
 *                                                                       *
 *************************************************************************/
#include "stdafx.h"
static char GPGPUInterpCode [] = "\n"
"// Define inputs from application.\n"
"struct appin\n"
"{\n"
"	float4 Position : POSITION;\n"
"	float4 Normal : NORMAL;\n"
"	float2 texCoord : TEXCOORD0;\n"
"	float2 vertexTexCoord : TEXCOORD1;\n"
"	float4 Color : COLOR0;\n"
"};\n"
"\n"
"\n"
"// Define outputs from vertex shader.\n"
"struct vertout\n"
"{\n"
"	float4 HPosition : POSITION;\n"
"	float4 Color : COLOR;\n"
"	float2 texCoord : TEXCOORD0;\n"
"};\n"
"\n"
"\n"
"// ************* fragment shader, pass 1 ****************\n"
"\n"
"void Interpolation(float2 idTexCoord : TEXCOORD1,\n"
"                uniform sampler2D interpolationTexture : TEXCOORD1,\n"
"                uniform sampler2D coarseDeformationTexture : TEXCOORD2,\n"
"                uniform float interpolationTextureSize,\n"
"                uniform float fineDeformationTextureSize,\n"
"                uniform float coarseDeformationTextureSize,\n"
"		 out float4 color : COLOR\n"
"		  )\n"
"{\n"
"\n"
"  // compute pixel coordinates and the vertex index\n"
"  float i = floor(idTexCoord.x * fineDeformationTextureSize); // 0-indexed column in the vertex texture image where this vertex is located\n"
"  float j = floor(idTexCoord.y * fineDeformationTextureSize); // 0-indexed row in the vertex texture image where this vertex is located\n"
"  float vertexIndex = j * fineDeformationTextureSize + i; // 0-index of the vertex corresponding to given interpolationTexCoord\n"
"\n"
"  // ------ compute deformation u of vertex with index 'vertexIndex' ----\n"
"\n"
"  float invInterpolationTextureSize = 1.0 / interpolationTextureSize;\n"
"  float J = floor(2*vertexIndex * invInterpolationTextureSize); // 0-indexed row in the interpolation texture\n"
"  float I = 2*vertexIndex - J * interpolationTextureSize; // 0-indexed column in the interpolation texture\n"
"\n"
"  // compute tet vertex deformations\n"
"  float4 integerIndices = tex2D(interpolationTexture, float2((I + 0.5)*invInterpolationTextureSize, (J + 0.5)*invInterpolationTextureSize));\n"
"  float4 barycentricCoords = tex2D(interpolationTexture, float2((I + 1.5)*invInterpolationTextureSize, (J + 0.5)*invInterpolationTextureSize));\n"
"  //float4 integerIndices = float4(1,2,3,4);\n"
"  //float4 barycentricCoords = float4(0.1, 0.2, 0.3, 0.5);\n"
"  //float4 barycentricCoords = tex2D(interpolationTexture, float2(0.5, 0.5));\n"
"  //float4 barycentricCoords = tex2D(coarseDeformationTexture, float2(0.5, 0.5));\n"
"  //float4 barycentricCoords = float4(1.0, 0.0, 0.0, 1.0);\n"
"\n"
"  //barycentricCoords = tex2D(interpolationTexture, float2(0.001953125, 0.0009765625));\n"
"\n"
"  float invCoarseDeformationTextureSize = 1.0 / coarseDeformationTextureSize;\n"
"\n"
"  float J0 = floor (integerIndices.x * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"  float I0 = integerIndices.x - J0 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"  float4 u0 = tex2D(coarseDeformationTexture, float2((I0 + 0.5) * invCoarseDeformationTextureSize, (J0 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"  float J1 = floor (integerIndices.y * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"  float I1 = integerIndices.y - J1 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"  float4 u1 = tex2D(coarseDeformationTexture, float2((I1 + 0.5) * invCoarseDeformationTextureSize, (J1 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"  float J2 = floor (integerIndices.z * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"  float I2 = integerIndices.z - J2 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"  float4 u2 = tex2D(coarseDeformationTexture, float2((I2 + 0.5) * invCoarseDeformationTextureSize, (J2 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"  float J3 = floor (integerIndices.w * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"  float I3 = integerIndices.w - J3 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"  float4 u3 = tex2D(coarseDeformationTexture, float2((I3 + 0.5) * invCoarseDeformationTextureSize, (J3 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"  // apply barycentric interpolation\n"
"  color = barycentricCoords.x * u0 + barycentricCoords.y * u1 + barycentricCoords.z * u2 + barycentricCoords.w * u3;\n"
"  //color = u3;\n"
"  //color = float4(barycentricCoords.x, 0, 0, 1);\n"
"  //color = float4(vertexIndex / 10000, 0, 0, 1);\n"
"  color.w = 1.0;\n"
"\n"
"  //color = float4(0.0, 3.0, 0.0, 1.0);\n"
"  //color = float4(0.0, interpolationTexCoord.x, 0.0, 0.0);\n"
"  //color = float4(0.0, interpolationTextureSize / 100, 0.0, 0.0);\n"
"  //color = float4(0.0, -3.0, 0.0, 1.0);\n"
"\n"
"}\n"
"\n"
"\n"
"// ************* fragment shader no texture, pass 2 ****************\n"
"void fragmentShaderPass2NoTexture(vertout IN,\n"
"		  out float4 color : COLOR\n"
"		  )\n"
"{\n"
"	color = IN.Color;\n"
"}\n"
"\n"
"\n"
"// ************* fragment shader with texture, pass 2 ****************\n"
"void fragmentShaderPass2(vertout IN,\n"
"		  out float4 color : COLOR,\n"
"		  uniform sampler2D texPass2 : TEXUNIT0\n"
"		  )\n"
"{\n"
"        //float2 uv = float2(0.5,0.5);\n"
"\n"
"	float4 texCol = tex2D(texPass2, IN.texCoord);\n"
"	//float4 texCol = tex2D(texPass2, uv);\n"
"\n"
"	color = IN.Color * texCol;\n"
"	//color = texCol;\n"
"	//color = IN.Color;\n"
"	//color = float4(0,0,1,0);\n"
"}\n"
"\n"
"// ************* vertex shader for triangles, standalone ****************\n"
"\n"
"vertout vertexShaderStandalone(appin IN,\n"
"                        float2 index01   : TEXCOORD1,\n"
"                        float2 index23   : TEXCOORD2,\n"
"                        float2 weights01 : TEXCOORD3,\n"
"                        float2 weights23 : TEXCOORD4,\n"
"                        uniform sampler2D coarseDeformationTexture : TEXUNIT1,\n"
"                        uniform float coarseDeformationTextureSize,\n"
" 			 uniform float4 Ka, \n"
"			 uniform float4 Kd, \n"
"			 uniform float4 Ks, \n"
"			 uniform float4 shininess, \n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float4x4 ModelViewIT,\n"
"			 uniform float4 LightPos1,\n"
"			 uniform float4 LightPos2,\n"
"			 uniform float4 LightPos3,\n"
"			 uniform float4 LightPos4,\n"
"			 uniform float Light1Intensity, \n"
"			 uniform float Light2Intensity, \n"
"			 uniform float Light3Intensity, \n"
"			 uniform float Light4Intensity, \n"
"			 uniform float AmbientIntensity \n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"       OUT.texCoord = IN.texCoord;\n"
"\n"
"       float4 integerIndices = float4(index01.x, index01.y, index23.x, index23.y);\n"
"       float4 barycentricCoords = float4(weights01.x, weights01.y, weights23.x, weights23.y);\n"
"       //integerIndices = float4(6,7,8,9);\n"
"       //barycentricCoords = float4(0.2, 0.3, 0.1, 0.4);\n"
"       //float4 integerIndices = float4(57,122,12,87);\n"
"       //float4 barycentricCoords = float4(1.0, 0.0, 0.0, 0.0);\n"
"       //float4 barycentricCoords = tex2D(interpolationTexture, float2(0.5, 0.5));\n"
"       //float4 barycentricCoords = tex2D(coarseDeformationTexture, float2(0.5, 0.5));\n"
"       //float4 barycentricCoords = float4(1.0, 0.0, 0.0, 1.0);\n"
"\n"
"       float invCoarseDeformationTextureSize = 1.0 / coarseDeformationTextureSize;\n"
"\n"
"       float J0 = floor (integerIndices.x * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I0 = integerIndices.x - J0 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u0 = tex2D(coarseDeformationTexture, float2((I0 + 0.5) * invCoarseDeformationTextureSize, (J0 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       float J1 = floor (integerIndices.y * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I1 = integerIndices.y - J1 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u1 = tex2D(coarseDeformationTexture, float2((I1 + 0.5) * invCoarseDeformationTextureSize, (J1 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       float J2 = floor (integerIndices.z * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I2 = integerIndices.z - J2 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u2 = tex2D(coarseDeformationTexture, float2((I2 + 0.5) * invCoarseDeformationTextureSize, (J2 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       float J3 = floor (integerIndices.w * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I3 = integerIndices.w - J3 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u3 = tex2D(coarseDeformationTexture, float2((I3 + 0.5) * invCoarseDeformationTextureSize, (J3 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       // apply barycentric interpolation\n"
"       //u0 = float4(1,0,0,0);\n"
"       //u1 = float4(1,0,0,0);\n"
"       //u2 = float4(1,0,0,0);\n"
"       //u3 = float4(1,0,0,0);\n"
"       float4 displacement = barycentricCoords.x * u0 + barycentricCoords.y * u1 + barycentricCoords.z * u2 + barycentricCoords.w * u3;\n"
"       //float4 displacement = u0;\n"
"       //float4 displacement = float4(barycentricCoords.x, 0, 0, 1);\n"
"       //float4 displacement = float4(integerIndices.x, 0, 0, 1);\n"
"       //float4 displacement = float4(vertexIndex / 10000, 0, 0, 1);\n"
"\n"
"       //float4 displacement = float4(0.0, 3.0, 0.0, 1.0);\n"
"       //float4 displacement = float4(0.0, -3.0, 0.0, 1.0);\n"
"       //float4 displacement = float4(0.0, 0.0, 0.0, 1.0);\n"
"\n"
"	//float4 displacement = float4(0, 0, IN.vertexTexCoord.x, 1.0);\n"
"	//float4 displacement = tex2D(vertexTex, IN.vertexTexCoord);\n"
"	//float4 displacement = tex2D(vertexTex, float2(0.5,0.5));\n"
"	//float4 displacement = float4(0.0, 0.0, -5.0, 1.0);\n"
"       //displacement.x *= 100;\n"
"       //displacement.y = 0.0;\n"
"       //displacement.z = 0.0;\n"
"       displacement.w = 1.0;\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	//float4 displacedPosition = IN.Position + float4(1.0, 0.0, 0.0, 1.0);\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"	// Transform normal from model-space to view-space.\n"
"	float3 normalVec = normalize(mul(ModelViewIT, IN.Normal).xyz);\n"
"\n"
"	float4 lighting = float4(0,0,0,0);\n"
"	float4 lightV;\n"
"	float3 lightVec;\n"
"	float3 eyeVec;\n"
"       float3 reflectedVec;\n"
"	float diffuse;\n"
"	float specular;\n"
"\n"
"\n"
"   	// *** ==== process light 1 === ***\n"
"	lightV = LightPos1 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light1Intensity,Light1Intensity,Light1Intensity,Light1Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 2 ==== ***\n"
"	lightV = LightPos2 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light2Intensity,Light2Intensity,Light2Intensity,Light2Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 3 ==== ***\n"
"	lightV = LightPos3 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light3Intensity,Light3Intensity,Light3Intensity,Light3Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 4 ==== ***\n"
"	lightV = LightPos4 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light4Intensity,Light4Intensity,Light4Intensity,Light4Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"   // *** === add material properties === ***\n"
"\n"
"	// Combine diffuse and specular contributions and\n"
"	// output the final vertex color.\n"
"       lighting.x += AmbientIntensity; \n"
"       //Ks = float4(0.8, 0.8, 0.8, 1.0) ;\n"
"	OUT.Color.rgba = lighting.x * Ka + lighting.y * Kd + lighting.z * Ks;\n"
"       //float len = sqrt(dot(IN.Normal, IN.Normal));\n"
"	//OUT.Color.rgb = float3(len, len, len);\n"
"	//OUT.Color.rgb = -IN.Normal;\n"
"	//OUT.Color.rgb = normalVec;\n"
"	//OUT.Color.rgb = float3(lighting.y, lighting.y, lighting.y);\n"
"	//OUT.Color.rgb = float3(Light3Intensity, Light3Intensity, Light3Intensity);\n"
"	//OUT.Color = Kd;\n"
"	//OUT.Color.a = 1.0;\n"
"	//OUT.Color.rgba = float4(1,0,0,1);\n"
"	//OUT.Color.rgba = float4(barycentricCoords.x,0,0,1);\n"
"	//OUT.Color.rgba = float4(displacement.x,0,0,1);\n"
"	//OUT.Color.rgba = 0.5 * float4(integerIndices.x,0,0,1);\n"
"	//OUT.Color.rgba = displacement;\n"
"\n"
"	return OUT;\n"
"}\n"
"\n"
"\n"
"// ************* vertex shader for triangles, pass 2 ****************\n"
"\n"
"vertout vertexShaderPass2(appin IN,\n"
"			 uniform sampler2D vertexTex : TEXUNIT1,\n"
" 			 uniform float4 Ka, \n"
"			 uniform float4 Kd, \n"
"			 uniform float4 Ks, \n"
"			 uniform float4 shininess, \n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float4x4 ModelViewIT,\n"
"			 uniform float4 LightPos1,\n"
"			 uniform float4 LightPos2,\n"
"			 uniform float4 LightPos3,\n"
"			 uniform float4 LightPos4,\n"
"			 uniform float Light1Intensity, \n"
"			 uniform float Light2Intensity, \n"
"			 uniform float Light3Intensity, \n"
"			 uniform float Light4Intensity, \n"
"			 uniform float AmbientIntensity \n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"\n"
"       OUT.texCoord = IN.texCoord;\n"
"	//float4 displacement = float4(0, 0, IN.vertexTexCoord.x, 1.0);\n"
"	float4 displacement = tex2D(vertexTex, IN.vertexTexCoord);\n"
"	//float4 displacement = tex2D(vertexTex, float2(0.5,0.5));\n"
"	//float4 displacement = float4(0.0, 0.0, -5.0, 1.0);\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	//float4 displacedPosition = IN.Position + float4(1.0, 0.0, 0.0, 1.0);\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"	// Transform normal from model-space to view-space.\n"
"	float3 normalVec = normalize(mul(ModelViewIT, IN.Normal).xyz);\n"
"\n"
"	float4 lighting = float4(0,0,0,0);\n"
"	float4 lightV;\n"
"	float3 lightVec;\n"
"	float3 eyeVec;\n"
"       float3 reflectedVec;\n"
"	float diffuse;\n"
"	float specular;\n"
"\n"
"\n"
"   	// *** ==== process light 1 === ***\n"
"	lightV = LightPos1 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light1Intensity,Light1Intensity,Light1Intensity,Light1Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 2 ==== ***\n"
"	lightV = LightPos2 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light2Intensity,Light2Intensity,Light2Intensity,Light2Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 3 ==== ***\n"
"	lightV = LightPos3 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light3Intensity,Light3Intensity,Light3Intensity,Light3Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 4 ==== ***\n"
"	lightV = LightPos4 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	//lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"	lightVec = normalize(lightV.xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light4Intensity,Light4Intensity,Light4Intensity,Light4Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"   // *** === add material properties === ***\n"
"\n"
"	// Combine diffuse and specular contributions and\n"
"	// output the final vertex color.\n"
"       lighting.x += AmbientIntensity; \n"
"       //Ks = float4(0.8, 0.8, 0.8, 1.0) ;\n"
"	OUT.Color.rgba = lighting.x * Ka + lighting.y * Kd + lighting.z * Ks;\n"
"       //float len = sqrt(dot(IN.Normal, IN.Normal));\n"
"	//OUT.Color.rgb = float3(len, len, len);\n"
"	//OUT.Color.rgb = -IN.Normal;\n"
"	//OUT.Color.rgb = normalVec;\n"
"	//OUT.Color.rgb = float3(lighting.y, lighting.y, lighting.y);\n"
"	//OUT.Color.rgb = float3(Light3Intensity, Light3Intensity, Light3Intensity);\n"
"	//OUT.Color = Kd;\n"
"	OUT.Color.a = 1.0;\n"
"	//OUT.Color.rgba = float4(1,0,0,1);\n"
"	//OUT.Color.rgba = displacement;\n"
"\n"
"	return OUT;\n"
"}\n"
"\n"
"// ************* vertex shader for triangles' shadow, pass 2 ****************\n"
"\n"
"vertout vertexShaderShadowPass2(appin IN,\n"
"			 uniform sampler2D tex : TEXUNIT0,\n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float ShadowIntensity\n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"\n"
"       OUT.texCoord = IN.texCoord;\n"
"	float4 displacement = tex2D(tex,IN.vertexTexCoord);\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"	OUT.Color.rgba = float4(ShadowIntensity,ShadowIntensity,ShadowIntensity,1);\n"
"\n"
"	return OUT;\n"
"}\n"
"// ************* vertex shader for triangles' shadow, pass 2, standalone ****************\n"
"\n"
"vertout vertexShaderShadowPass2Standalone(appin IN,\n"
"                        float2 index01   : TEXCOORD1,\n"
"                        float2 index23   : TEXCOORD2,\n"
"                        float2 weights01 : TEXCOORD3,\n"
"                        float2 weights23 : TEXCOORD4,\n"
"                        uniform sampler2D coarseDeformationTexture : TEXUNIT1,\n"
"                        uniform float coarseDeformationTextureSize,\n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float ShadowIntensity\n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"       OUT.texCoord = IN.texCoord;\n"
"\n"
"       float4 integerIndices = float4(index01.x, index01.y, index23.x, index23.y);\n"
"       float4 barycentricCoords = float4(weights01.x, weights01.y, weights23.x, weights23.y);\n"
"       //integerIndices = float4(6,7,8,9);\n"
"       //barycentricCoords = float4(0.2, 0.3, 0.1, 0.4);\n"
"       //float4 integerIndices = float4(57,122,12,87);\n"
"       //float4 barycentricCoords = float4(1.0, 0.0, 0.0, 0.0);\n"
"       //float4 barycentricCoords = tex2D(interpolationTexture, float2(0.5, 0.5));\n"
"       //float4 barycentricCoords = tex2D(coarseDeformationTexture, float2(0.5, 0.5));\n"
"       //float4 barycentricCoords = float4(1.0, 0.0, 0.0, 1.0);\n"
"\n"
"       float invCoarseDeformationTextureSize = 1.0 / coarseDeformationTextureSize;\n"
"\n"
"       float J0 = floor (integerIndices.x * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I0 = integerIndices.x - J0 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u0 = tex2D(coarseDeformationTexture, float2((I0 + 0.5) * invCoarseDeformationTextureSize, (J0 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       float J1 = floor (integerIndices.y * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I1 = integerIndices.y - J1 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u1 = tex2D(coarseDeformationTexture, float2((I1 + 0.5) * invCoarseDeformationTextureSize, (J1 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       float J2 = floor (integerIndices.z * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I2 = integerIndices.z - J2 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u2 = tex2D(coarseDeformationTexture, float2((I2 + 0.5) * invCoarseDeformationTextureSize, (J2 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       float J3 = floor (integerIndices.w * invCoarseDeformationTextureSize); // 0-indexed row in the coarseDeformation texture\n"
"       float I3 = integerIndices.w - J3 * coarseDeformationTextureSize; // 0-indexed column in the coarseDeformation texture\n"
"       float4 u3 = tex2D(coarseDeformationTexture, float2((I3 + 0.5) * invCoarseDeformationTextureSize, (J3 + 0.5) * invCoarseDeformationTextureSize));\n"
"\n"
"       // apply barycentric interpolation\n"
"       //u0 = float4(1,0,0,0);\n"
"       //u1 = float4(1,0,0,0);\n"
"       //u2 = float4(1,0,0,0);\n"
"       //u3 = float4(1,0,0,0);\n"
"       float4 displacement = barycentricCoords.x * u0 + barycentricCoords.y * u1 + barycentricCoords.z * u2 + barycentricCoords.w * u3;\n"
"       //float4 displacement = u0;\n"
"       //float4 displacement = float4(barycentricCoords.x, 0, 0, 1);\n"
"       //float4 displacement = float4(integerIndices.x, 0, 0, 1);\n"
"       //float4 displacement = float4(vertexIndex / 10000, 0, 0, 1);\n"
"\n"
"       //float4 displacement = float4(0.0, 3.0, 0.0, 1.0);\n"
"       //float4 displacement = float4(0.0, -3.0, 0.0, 1.0);\n"
"       //float4 displacement = float4(0.0, 0.0, 0.0, 1.0);\n"
"\n"
"	//float4 displacement = float4(0, 0, IN.vertexTexCoord.x, 1.0);\n"
"	//float4 displacement = tex2D(vertexTex, IN.vertexTexCoord);\n"
"	//float4 displacement = tex2D(vertexTex, float2(0.5,0.5));\n"
"	//float4 displacement = float4(0.0, 0.0, -5.0, 1.0);\n"
"       //displacement.x *= 100;\n"
"       //displacement.y = 0.0;\n"
"       //displacement.z = 0.0;\n"
"       displacement.w = 1.0;\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"	OUT.Color.rgba = float4(ShadowIntensity,ShadowIntensity,ShadowIntensity,1);\n"
"\n"
"	return OUT;\n"
"}\n"
"// ************* vertex shader for triangles, pass 2 ****************\n"
"\n"
"vertout vertexShaderPass2WithDefoNormals(appin IN,\n"
"			 uniform sampler2D tex : TEXUNIT0,\n"
"			 uniform sampler2D texNormals : TEXUNIT3,\n"
"			 //uniform float q0,\n"
" 			 uniform float4 Ka, \n"
"			 uniform float4 Kd, \n"
"			 uniform float4 Ks, \n"
"			 uniform float4 shininess, \n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float4x4 ModelViewIT,\n"
"			 uniform float4 LightPos1,\n"
"			 uniform float4 LightPos2,\n"
"			 uniform float4 LightPos3,\n"
"			 uniform float4 LightPos4,\n"
"			 uniform float Light1Intensity, \n"
"			 uniform float Light2Intensity, \n"
"			 uniform float Light3Intensity, \n"
"			 uniform float Light4Intensity, \n"
"			 uniform float AmbientIntensity \n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"\n"
"       OUT.texCoord = IN.texCoord;\n"
"	//float4 displacement = tex2D(tex,float2(0.5,0.5));\n"
"	float4 displacement = tex2D(tex,IN.vertexTexCoord);\n"
"	//float4 displacement = float4(-5.0, 3.0, 0.0, 1.0);\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"   float4 deltaNormal = tex2D(texNormals,IN.vertexTexCoord);\n"
"   float3 dynamicNormal = normalize(IN.Normal.xyz + deltaNormal.xyz);\n"
"   float4 normal4;\n"
"   normal4.xyz = dynamicNormal;\n"
"   normal4.w = 1.0;\n"
"	// Transform normal from model-space to view-space.\n"
"	float3 normalVec = normalize(mul(ModelViewIT, normal4).xyz);\n"
"\n"
"	float4 lighting = float4(0,0,0,0);\n"
"	float4 lightV;\n"
"	float3 lightVec;\n"
"	float3 eyeVec;\n"
"   float3 reflectedVec;\n"
"	float diffuse;\n"
"	float specular;\n"
"\n"
"\n"
"   // *** ==== process light 1 === ***\n"
"	lightV = LightPos1 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light1Intensity,Light1Intensity,Light1Intensity,Light1Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 2 ==== ***\n"
"	lightV = LightPos2 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light2Intensity,Light2Intensity,Light2Intensity,Light2Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 3 ==== ***\n"
"	lightV = LightPos3 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light3Intensity,Light3Intensity,Light3Intensity,Light3Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"	// *** ==== process light 4 ==== ***\n"
"	lightV = LightPos4 - displacedPosition;\n"
"	lightV.w = 1.0;\n"
"	lightVec = normalize(mul(ModelViewIT, lightV).xyz);\n"
"\n"
"	eyeVec = float3(0.0, 0.0, 1.0);\n"
"	//float3 halfVec = normalize(lightVec + eyeVec);\n"
"	reflectedVec = normalize(lightVec - 2*(lightVec-dot(lightVec,normalVec)*normalVec));\n"
"\n"
"	// Calculate diffuse component\n"
"	diffuse = dot(normalVec, lightVec);\n"
"\n"
"	// Calculate specular component.\n"
"	//specular = dot(normalVec, halfVec);\n"
"	specular = dot(normalVec, reflectedVec);\n"
"\n"
"	// Use the lit function to compute lighting vector from diffuse and specular values.\n"
"	lighting += float4(Light4Intensity,Light4Intensity,Light4Intensity,Light4Intensity) \n"
"	  * lit(diffuse, specular, shininess);\n"
"\n"
"\n"
"   // *** === add material properties === ***\n"
"\n"
"	// Combine diffuse and specular contributions and\n"
"	// output final vertex color.\n"
"   lighting.x += AmbientIntensity; \n"
"   //Ks = float4(0.8, 0.8, 0.8, 1.0) ;\n"
"	OUT.Color.rgba = lighting.x * Ka + lighting.y * Kd + lighting.z * Ks;\n"
"	OUT.Color.a = 1.0;\n"
"\n"
"	return OUT;\n"
"}\n"
"\n"
"// ************* vertex shader for points, pass 2 ****************\n"
"\n"
"vertout vertexShader_Points_Pass2(appin IN,\n"
"		     uniform sampler2D tex : TEXUNIT0,\n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float4x4 ModelViewIT\n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"\n"
"	float4 displacement = tex2D(tex,IN.vertexTexCoord);\n"
"	//float4 displacement = 0;\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"	OUT.Color = IN.Color;\n"
"\n"
"	//OUT.Color = float4(0,0,1,1);\n"
"\n"
"	return OUT;\n"
"}\n"
"\n"
"// ************* vertex shader for edges, pass 2 ****************\n"
"\n"
"vertout vertexShader_Edges_Pass2(appin IN,\n"
"		     uniform sampler2D tex : TEXUNIT0,\n"
"			 uniform float4x4 ModelViewProj,\n"
"			 uniform float4x4 ModelViewIT\n"
"			 )\n"
"{\n"
"	vertout OUT;\n"
"\n"
"	float4 displacement = tex2D(tex,IN.vertexTexCoord);\n"
"	//float4 displacement = 0;\n"
"\n"
"	float4 displacedPosition = IN.Position + displacement;\n"
"	displacedPosition.w = 1.0;\n"
"\n"
"	// Transform vertex position into homogenous clip-space.\n"
"	OUT.HPosition = mul(ModelViewProj, displacedPosition);\n"
"\n"
"	OUT.Color = IN.Color;\n"
"\n"
"	//OUT.Color = float4(0,0,1,1);\n"
"\n"
"	return OUT;\n"
"}\n";
