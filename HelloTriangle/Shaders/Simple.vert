const char* SimpleVertexShader = STRINGIFY(

attribute vec4 vPosition; 
void main()                
{                          
    gl_Position = vPosition; 
}                            

);
