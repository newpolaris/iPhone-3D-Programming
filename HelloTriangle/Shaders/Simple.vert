const char* SimpleVertexShader = STRINGIFY(

attribute vec4 vPosition; 
attribute vec4 SourceColor;
varying vec4 DestinationColor;
uniform mat4 Projection;
uniform mat4 Modelview;

void main()                
{                          
    DestinationColor = SourceColor;
    gl_Position = Projection * Modelview * vPosition; 
}                            

);
