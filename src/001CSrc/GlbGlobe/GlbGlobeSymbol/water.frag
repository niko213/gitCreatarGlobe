//uniform mat4 gl_ModelViewMatrix;                       // ģ����ͼ�任����
//uniform mat4 osg_ViewMatrixInverse;
uniform mat3 osg_NormalMatrix;
uniform mat4 gl_ModelViewMatrix;                // ģ����ͼ�任����
uniform mat4 osg_ViewMatrix;                    // ��ͼ����
uniform mat4 osg_ViewMatrixInverse;             // ��ͼ����
uniform float osg_FrameTime;

uniform vec2 PolygonCenter;                     // ���������
uniform vec2 PolygonWeightAndHeight;             // ����δ�С

uniform vec2 PolygonRotateCenter;                // �������ͼ��������
uniform vec2 PolygonRotateWeightAndHeight;       // �������ת�Ժ�Ĵ�С
//uniform vec3 sunPos;
uniform int PolygonOpacity;                      // ����β�͸����ϵ����0-��ȫ͸����100-Ϊ��ȫ��͸����
uniform vec4 PolygonColor;                       // �������ɫ(0-255)
uniform bool PolygonUseTex;                      // �����ʹ����ͼ
uniform bool PolygonSelected;
uniform bool PolygonBlink;

uniform float windSpeed;                         // ����
uniform float windDirection;                     // ����

uniform bool waterPolygon_EnableReflections;
uniform bool waterPolygon_EnableRefractions;
uniform sampler2D   WaterPolygon_HeightMap;
uniform sampler2D   WaterPolygon_ReflectionMap;
uniform sampler2D   WaterPolygon_CameraRefractionMap;           // ���������Ⱦ������ͼ
uniform sampler2D   WaterPolygon_SpecifyRefractionMap;          // ָ����������ͼ

varying vec3 eyePos;
varying vec3 eyeDirction;
varying vec4 vertexCoords_inObj;
varying vec4 vertexCoords_inMainCam;
varying vec4 vertexCoords_inRefCam;
varying vec4 vertexCoords_inRefCamEx;

//
float getHeight(sampler2D heightMap, vec2 texcoord ,float frameTime)
{
    //texcoord*=5.0;
	vec2 texcoordH=texcoord;
	float wavelen=30.0;	
	float winSpeed=windSpeed/250.0;//float speed=0.02;
	//winSpeed=0.0;
	float PI_Factor=3.1415926/180.0;
	float angs=windDirection/PI_Factor;
	vec2 windDir=vec2(sin(angs),cos(angs));
	vec2 windTexOffset=windDir*winSpeed*frameTime;
	texcoordH += windTexOffset;	
	float s1 = texture2D(heightMap,texcoordH+0.5).r;
	float s2 = texture2D(heightMap,texcoordH).r;
	
	//return s2;
	
	// float waveSpeed=1.0;
	// waveSpeed*=(1.0+winSpeed);
	// vec2 waveTexOffset=windDir*waveSpeed*frameTime;
	// vec2 texcoordTmp = texcoord + waveTexOffset;
	// float s=0.2;
	// return cos(texcoordTmp.y)*s1+ cos(texcoordTmp.x)*s2;
	
	float waveSpeed=0.08;
	waveSpeed*=(1.0+winSpeed);
	vec2 waveTexOffset=windDir*waveSpeed*frameTime;
	vec2 texcoordTmp = texcoord + waveTexOffset;
	float s=0.2;
	return (cos((texcoordTmp.y*1.0+s1*s)*wavelen)* cos((texcoordTmp.x*0.5-s2*s)*wavelen)*0.5+0.5)+s2;
	// float wh = (sin(length(texcoord))*0.5+0.5);
	// return mix(s1,s2,wh);
}
float getHeight0(sampler2D heightMap,vec2 texcoord,float frameTime)
{
    float speed=0.10;
	speed=windSpeed/250.0;
	float PI_Factor=3.1415926/180.0;
	vec2 windDir=vec2(sin(windDirection/PI_Factor),cos(windDirection/PI_Factor));
	texcoord += windDir*speed*frameTime;	
	//texcoord -= frameTime*speed;
	float s1 = texture2D(heightMap,texcoord+0.5).r;
	float s2 = texture2D(heightMap,texcoord).r;
	//Ч��1
	float s = 0.2;
	return (cos((texcoord.y+s1*s)*30.0)* cos((texcoord.x-s2*s)*30.0)*0.5+0.5);
}
float getHeight1(sampler2D heightMap,vec2 texcoord,float frameTime)
{
    float speed=0.10;
	speed=windSpeed/250.0;
	//texcoord = texcoord*vec2(1.0,2.0);
	texcoord.y -= frameTime*speed;
	//texcoord -= frameTime*speed;
	float s1 = texture2D(heightMap,texcoord+0.5).r;
	float s2 = texture2D(heightMap,texcoord).r;
	
	texcoord.y-=frameTime*speed;
	//Ч��1
	float s = 0.2;
	return (cos((texcoord.y+s1*s)*30.0)* cos((texcoord.x-s2*s)*30.0)*0.5+0.5);
	
	//Ч��2
	//float wh = (sin(texcoord.y*30.0)*0.5+0.5);		
	//return 1.0-mix(s1,s2,wh);	
}
//ȡ�ض�λ��ˮ����Ը߶�
float getHeight2(sampler2D heightMap, vec2 texcoord ,float frameTime)
{
	vec2 coordcache=texcoord;
	//texcoord-=frameTime*0.1;
	float wavelen=30.0;	wavelen=10.0;
	//float speed=0.02;
	float speed=windSpeed/250.0;
	float PI_Factor=3.1415926/180.0;
	vec2 windDir=vec2(sin(windDirection/PI_Factor),cos(windDirection/PI_Factor));
	texcoord += windDir*speed*frameTime;
	//texcoord+=frameTime*speed;
	float wh = (sin((texcoord.y-texcoord.x)*wavelen)* sin((texcoord.y+texcoord.x)*wavelen)*0.5+0.5);	
	float s1 = texture2D(heightMap,coordcache-frameTime*speed* vec2(1.0,1.0)+0.5).r;
	float s2 = texture2D(heightMap,coordcache-frameTime*speed* vec2(1.0,1.0)).r;
	//return s2;
	return mix(s1,s2,wh);
}

//vec3 spec = computeLighting(vec3(0.0,400,-400), specularColor,light_Color, 300.0,vertexCoords_inCam,normal1,eyePos);
vec3 computeLighting(vec3 lightPosition,
                       vec3 lightColor,
                       vec3 Kd,
                       float  shininess,
                       vec3 P,
                       vec3 N,
                       vec3 eyePosition)
{
  // Compute the diffuse lighting
  vec3 L = normalize(lightPosition - P);

  float diffuseLight = max(dot(N, L), 0.0);
  vec3 diffuseResult = Kd * lightColor * diffuseLight;
  
  // Compute the specular lighting
  vec3 V = normalize(eyePosition - P);
  vec3 H = normalize(L + V);
  vec3 specularLight = lightColor * pow(max(dot(N, H), 0.0),shininess);
  if (diffuseLight <= 0.0) 
      specularLight = vec3(0.0,0.0,0.0);
  vec3 specularResult = lightColor * specularLight;
  return specularResult;//����ˮ�棬ֻ��Ҫ���ظ߹⼴��
  //return diffuseResult + specularResult+0.1;
}

// Main Program
void main( void )
{
    vec4 water_color = vec4(0.0,0.3,0.13,0.1);
	//water_color.a=PolygonOpacity/100.0;
	//vec4 water_color = vec4(0.15625,0.58593,0.78125,0.1); //ˮ�������ɫ
    //vec4 water_color = vec4( 0.160784, 0.231372, 0.325490, 0.1); //ˮ�������ɫ	
	water_color.rgb = PolygonColor.rgb/255.0;	
	//return;       // ����ˮ����ɫ
	vec4 light_Color = vec4(0.9,0.8,0.7,1.0);                 //���յ���ɫ
	
    float camPosSize=length(eyePos);
	if(abs(camPosSize)<0.000001)
	    discard;
	gl_FragColor.rgb=normalize(eyePos);
	gl_FragColor.a=1.0;
	//return;       // �������λ��
	float dis = length(eyeDirction);
	
	vec3 water_eyeDirction = normalize(eyeDirction);      // ���߷���
	gl_FragColor.rgb=water_eyeDirction;
	gl_FragColor.a=1.0;
	//return;       // �������߷���
	
	if(abs(vertexCoords_inObj.w)<0.000001)
	    discard;
	vec2 uv_obj = vertexCoords_inObj.xy / vertexCoords_inObj.w;
	uv_obj = vertexCoords_inObj.xy/5.0;	
	//uv_obj = clamp((uv_obj + 1.0) * 0.5, 0.0, 1.0);
	//uv_obj = clamp(uv_obj, 0.0, 1.0);
	//uv_obj = fract(uv_obj);	
	
	if(abs(vertexCoords_inMainCam.w)<0.000001)
	    discard;
    vec2 uv_cam = vertexCoords_inMainCam.xy / vertexCoords_inMainCam.w;
	uv_cam = clamp((uv_cam + 1.0) * 0.5, 0.0, 1.0);       // ����������������
	
	if(abs(vertexCoords_inRefCam.w)<0.000001)
	    discard;
	vec2 uv_refcam = vertexCoords_inRefCam.xy / vertexCoords_inRefCam.w;
	uv_refcam = clamp((uv_refcam + 1.0) * 0.5, 0.0, 1.0); // ���㷴����������		
		
	vec2 mapsize = vec2(247.0,244.0);                     //heightMap����
	//vec2 mapsize = vec2(512.0,512.0);                     //heightMap����
	vec2 ds = 1.0/mapsize;
	
	float ratio = 1.1*(1.0+windSpeed);	//����ǿ�ȣ����Ʒ���ƫתǿ�ȣ�	
	ratio*=(pow(2.718,-dis)+1.0);
	//ratio = 1.0;	//����ǿ�ȣ����Ʒ���ƫתǿ�ȣ�	
	//ratio = min(ratio*20.0/dis,ratio);//������Ч��
	//ratio=1.3;	//����ǿ�ȣ����Ʒ���ƫתǿ�ȣ�
	//ratio = min(ratio*7.0/dis,ratio);//������Ч��
	
	//���㷨��
	float sam = 1.3;//�߶�ͼȡ�����ȣ�ֵԽ��ˮ��ϸ��Խ�٣�Խƽ��
	float tt=2.0-pow(2.718,-dis*3.0);
	sam*=tt;
	//sam*=tt*1.1;
	//sam*=tt*1.3;
	//sam=1.3;
	
	vec2 uv_nor=gl_TexCoord[0].xy;   // ������������
	uv_nor-=PolygonCenter;
    uv_nor/=PolygonWeightAndHeight;	
	uv_nor+=0.5;
	//uv_nor=uv_obj.xy;
	uv_nor=uv_obj.xy/sam;
	float h0 = getHeight(WaterPolygon_HeightMap,uv_nor,osg_FrameTime);  // ����ռ�
	float h1 = getHeight(WaterPolygon_HeightMap,uv_nor+vec2(ds.x*sam),osg_FrameTime);
	float h2 = getHeight(WaterPolygon_HeightMap,uv_nor+vec2(0.0,ds.y*sam),osg_FrameTime);
	// float h0 = getHeight(WaterPolygon_HeightMap,uv_nor,osg_FrameTime); // ������������
	// float h1 = getHeight(WaterPolygon_HeightMap,uv_nor+vec2(ds.x*sam,0.0),osg_FrameTime);
	// float h2 = getHeight(WaterPolygon_HeightMap,uv_nor+vec2(0.0,ds.y*sam),osg_FrameTime);
    gl_FragColor = vec4(h0,h1,h2,1.0);
	//gl_FragColor = vec4(h0,h0,h0,1.0);
	//return;	    //���Ը߶�ͼ
	//vec3 normal = cross(vec3(0.0,ratio*(h2-h0),sam),vec3(sam,ratio*(h1-h0),0.0));// y������
	vec3 normal = cross(vec3(sam,0.0,ratio*(h1-h0)),vec3(0.0,sam,ratio*(h2-h0)));// z������
	//normal=normal.yzx;	
	//normal = normalize(normal);
	gl_FragColor.rgb = normal;
	gl_FragColor.a=1.0;	
	//normal = normalize(normal);
	//return;       // ���Է���ͼ	
	
	vec2 uv=gl_TexCoord[1].xy;   // ������������
	uv-=PolygonRotateCenter;
    uv/=PolygonRotateWeightAndHeight;	
	uv+=0.5;
	// ����������ɫ
	vec4 refract_Color = water_color;	
	if(waterPolygon_EnableRefractions)// ʹ�����������ͼ
	{
		refract_Color = texture2D(WaterPolygon_CameraRefractionMap, uv_cam+normal.xy*0.08);// ���������ͼ
		refract_Color = mix(refract_Color,water_color,water_color.a); //��ˮ����ɫ���
	}
    else
	{
		 if(PolygonUseTex)
		 {	
			 //refract_Color = texture2D(WaterPolygon_SpecifyRefractionMap, uv+normal.xz*0.08);  // �ⲿ���������ͼ
			 refract_Color = texture2D(WaterPolygon_SpecifyRefractionMap, uv*4.0+normalize(normal).xy*0.08);  // �ⲿ���������ͼ		 
		     refract_Color = mix(refract_Color,water_color,water_color.a); //��ˮ����ɫ���
         }	
	     else
	     {
	         refract_Color = water_color;
	     }		
	}
	//refract_Color = water_color;  //���ԣ�����������ɫ
	gl_FragColor = refract_Color;	
	gl_FragColor.a =1.0;
	//return;       //����������ɫ
	
	// ���㷴����ɫ
	vec4 reflect_color = vec4(0.8,0.8,0.8,0.8);    // ��ʹ�÷������ɫ	
	if(waterPolygon_EnableReflections)
	{
	     //reflect_color= texture2D(water_ReflectionMap, uv_cam);
	     //reflect_color= texture2D(water_ReflectionMap, uv_refcam);
	     reflect_color = texture2D(WaterPolygon_ReflectionMap, uv_refcam+normal.xy*0.0051);
		 //reflect_color = texture2D(WaterPolygon_ReflectionMap, uv_refcam);
	}
	else
	{
	     reflect_color = vec4(0.8,0.8,0.8,0.8);  // ���Բ�ʹ�÷������ɫ
		 //reflect_color = water_color;    // ��ʹ�÷������ɫ
	}
	//reflect_color = vec4(0.8,0.8,0.8,0.8);  // ���Բ�ʹ�÷������ɫ
	gl_FragColor = reflect_color;
	//return;       //���Է�����ɫ	
	
	// ��������������ɫ
	float f0 = 0.257;	
	//float frenelFactor= f0 + (1.1-f0)* pow(1.0 - dot(-normalize(-water_eyeDirction.yzx), normalize(normal)),3.0);//(������y�����ϵ�����ϵ)	
	float frenelFactor= f0 + (1.1-f0)* pow(1.0 - dot(-normalize(-water_eyeDirction), normalize(normal)),3.0);//(������z�����ϵ�����ϵ,ԭ����osg��glsl������ϵ�����᲻һ��)
	frenelFactor = max(0.0,min(0.9,frenelFactor));	
	gl_FragColor = frenelFactor;
	gl_FragColor.a=PolygonOpacity/100.0;
	//return;       //���Է�����ϵ��
	vec4 frenel_color = mix(refract_Color,reflect_color,frenelFactor);	
	gl_FragColor = frenel_color;
	gl_FragColor.a=PolygonOpacity/100.0;
	
	
	if(PolygonSelected)
	{
	    gl_FragColor.rgb*=vec3(0.3,0.85,0.3);
	}	
	if(PolygonBlink)
	{	    
		int frameTimeFloor=int(floor(osg_FrameTime));		
	    frameTimeFloor=frameTimeFloor % 2;
	    if(frameTimeFloor==1)
	    {	
	        gl_FragColor.rgb+=vec3(0.85,0.25,0.25);
	    }
		else
		{
		    gl_FragColor.rgb+=vec3(0.25,0.85,0.85);
		}
	}
	
	return; // ����ֻ�з����������ɫ
	
	
	// ����߹�
	vec3 specularColor = light_Color.rgb;//=vec3(1,0.5,0); //�߹���ɫ
	vec3 normal1 = normal;
	normal1.y += 0.1;
	normal1 = normalize(normal1);
	vec3 spec = computeLighting(vec3(0.0,40000000.0,-40000000.0), specularColor,light_Color, 300.0,vertexCoords_inMainCam.rgb,normal1,eyePos);
	gl_FragColor.rgb = spec;
	gl_FragColor.a=1.0;
	//return;  //���Ը߹�
	
	// �ϳ������ɫ
	gl_FragColor = frenel_color;
	gl_FragColor.rgb *= light_Color;
	gl_FragColor.rgb += spec;
	gl_FragColor = min(gl_FragColor,1.0);
	return;
	
	// gl_FragColor = water_color;
	// gl_FragColor = water_color + reflect_color;
	// gl_FragColor = reflect_color;
	// gl_FragColor.w = 1.0;
	// gl_FragColor = refract_Color;
	// gl_FragColor.rgb *=2.0;
	// gl_FragColor.a *=1.0;
}


