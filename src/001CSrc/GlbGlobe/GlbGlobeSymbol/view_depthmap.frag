//#version 100
uniform mat4 gl_ModelViewMatrix;                // 模型视图变换矩阵
//uniform mat4 gl_ModelMatrix;                  // 模型变换矩阵
uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ViewMatrix;                   // 视图变换矩阵

uniform float osg_FrameTime;
uniform mat4 osg_ViewMatrix;                    // 视图矩阵
uniform mat4 osg_ViewMatrixInverse;             // 视图矩阵逆矩阵
uniform mat4 osg_ProjectionMatrix;              // 投影矩阵
//uniform mat4 osg_ProjectionMatrixInverse;

uniform sampler2D osgShadow_baseTexture;
uniform sampler2DShadow osgShadow_shadowTexture;

void main(void)
{
     vec4 color = gl_Color * texture2D( osgShadow_baseTexture, gl_TexCoord[0].xy ); 	
	 gl_FragColor = color;
	 if(abs(gl_TexCoord[1].w)<0.000001)
	     return;
	 vec4 shadowUVW=gl_TexCoord[1]/gl_TexCoord[1].w;
	 if(  shadowUVW.x >= 0.0 && shadowUVW.x <= 1.0//
       && shadowUVW.y >= 0.0 && shadowUVW.y <= 1.0
	   && shadowUVW.z >= 0.0 && shadowUVW.z <= 1.0
	     )
     { 
		 float splitDepth=shadow2DProj( osgShadow_shadowTexture, gl_TexCoord[1] ).r;
		 if(splitDepth>0.9)
		 {
			 gl_FragColor = color + vec4(0.0,0.60,0.0,0.0);
		 }
		 else if(splitDepth<0.1)//不可见
		 {
			 gl_FragColor = color + vec4(0.60,0.0,0.0,0.0);
		 }
     }
}

