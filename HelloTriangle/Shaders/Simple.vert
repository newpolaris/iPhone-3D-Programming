const char* SimpleVertexShader = STRINGIFY(

attribute vec4 vPosition; 
attribute vec4 SourceColor;
varying vec4 DestinationColor;

void main()                
{                          
    DestinationColor = SourceColor;
    gl_Position = vPosition; 
}                            

);
