#version 330
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.


//**texturing uniform

uniform sampler2D u_Texture; // The texture to be read from by this shader
uniform int u_Time; //time variable to animate WATER and LAVA blocks

//***Fog
uniform vec4 u_Player; //variable used to generate fog around player

uniform vec4 u_Color; // The color with which to render this instance of geometry.

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
//in vec4 fs_Col;

//**texturing in
in vec4 fs_UV;

//add a flag attribute to see if the block is animated

//in vec4 fs_CameraPos;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

//constants
const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

//rotation of sun on it's x axis
vec4 sunRotation(vec3 position, float angle){
    return vec4(position.x, cos(angle) *
                position.y +- sin(angle) * position.z, sin(angle) * position.y + cos(angle) * position.z, 0.0f);
}

//compute sun position based on time
vec3 computeSunPosition(float time) {
    time = time / 24.0;
    // Calculate the angle of the sun based on the time of day
    float angle = time * TWO_PI; // Full circle over a period of time
    // Compute sun position here (modify as needed)
    return vec3(normalize(sunRotation(normalize(vec3(0, -1.0, 0)), angle)));
}

vec4 setTextureColor(float r, float g, float b, vec4 textureColor) {
    textureColor[0] = r;
    textureColor[1] = g;
    textureColor[2] = b;

    return textureColor;
}

vec4 editBlockColors(vec2 uv, vec4 textureColor) {
    vec4 newTextureColor = textureColor;

    if ((uv.x >= 3.f/16.f && uv.x <= 4.f/16.f) && uv.y >= 15.f/16.f) {
        // Grass side.
        if (newTextureColor[0] < 0.5) {
            newTextureColor = setTextureColor(0.0431f, 0.51373f, 0.23137f, newTextureColor);
        }
    } else if ((uv.x >= 8.f/16.f && uv.x <= 9.f/16.f) && (uv.y >= 13.f/16.f && uv.y <= 14.f/16.f)) {
        // Grass top.
        newTextureColor = setTextureColor(newTextureColor[0] * 0.2f, newTextureColor[1] * 0.8f, newTextureColor[2] * 0.6f, newTextureColor);
    } else if ((uv.x >= 2.f/16.f && uv.x <= 3.f/16.f) && uv.y >= 15.f/16.f) {
        // Dirt block.
        if (newTextureColor[0] < 0.5) {
            newTextureColor = setTextureColor(newTextureColor[0] = 0.0431f, newTextureColor[1] = 0.51373f, newTextureColor[2] = 0.23137f, newTextureColor);
        }
    } else if ((uv.x >= 2.f/16.f && uv.x <= 3.f/16.f) && (uv.y >= 8.f/16.f && uv.y <= 9.f/16.f)) {
        // Black rock.
        newTextureColor = setTextureColor(newTextureColor[0], newTextureColor[1] * 1.2, newTextureColor[2] * 1.3, newTextureColor);
    } else if ((uv.x >= 2.f/16.f && uv.x <= 3.f/16.f) && (uv.y >= 2.f/16.f && uv.y <= 3.f/16.f)) {
        // Orange rock.
        newTextureColor = setTextureColor(newTextureColor[0], newTextureColor[1] * 0.8, 0.0, newTextureColor);
    } else if ((uv.x >= 5.f/16.f && uv.x <= 6.f/16.f) && (uv.y >= 13.f/16.f && uv.y <= 14.f/16.f)) {
        // Cave blocks.
        newTextureColor = setTextureColor(newTextureColor[0] * 0.2, newTextureColor[1] * 1.6, newTextureColor[2] * 1.4, newTextureColor);
    } else if ((uv.x >= 7.f/16.f && uv.x <= 8.f/16.f) && (uv.y >= 9.f/16.f && uv.y <= 10.f/16.f)) {
        // Teal mushroom.
        newTextureColor = setTextureColor(newTextureColor[0] * 0.1, newTextureColor[1] * 2, newTextureColor[2] * 2, newTextureColor);
    } else if ((uv.x >= 8.f/16.f && uv.x <= 9.f/16.f) && (uv.y >= 9.f/16.f && uv.y <= 10.f/16.f)) {
        // Mud blocks for mushroom fields.
        newTextureColor = setTextureColor(newTextureColor[0] * 1.2, newTextureColor[1] * 1.2, newTextureColor[2] * 1.2, newTextureColor);
    }

    return newTextureColor;
}

void main()
{
    vec2 uv = fs_UV.xy;

    //WATER OR LAVA
    vec2 offset = vec2(0, 0);

    // animated water / lava
    if (fs_UV.z != 0) {
        float oscillation = (sin(u_Time * 0.02) + 1) / 2; // oscillates between -1 and 1
        offset.x = oscillation * 100 * 0.005 / 10; // scale to 100 units
        uv -= offset;
    }

    vec4 textureColor = texture(u_Texture, uv); // Sample the texture

    // Modify texture colors depending on block type.
    textureColor = editBlockColors(uv, textureColor);

    // Material base color (before shading)

    vec4 diffuseColor = textureColor;     // diffuse color with the texture color
    //**UNCOMMENT TO LOAD TEXTURE
        //vec4 diffuseColor = fs_Col;
        if (diffuseColor.a < 0.01f){
            discard;
        }

        diffuseColor = diffuseColor;
        //**Block lighting reflected by sun
        const vec3 sunColor = vec3(255, 255, 190) / 255.0;
        vec3 sunDir = normalize(computeSunPosition(mod(u_Time * 0.01, 24.0)));

        // Calculate the diffuse term for Lambert shading
        float diffuseTerm = dot(normalize(fs_Nor), normalize(vec4(sunDir, 0.0)));
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.2;

        //**difuseTerm * diffuseColor
        //**ambient term * skyColor
        //diffuseterm and ambient Term are inverses

        float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black

        //float diffuseTerm = dot(normalize(fs_Nor), normalize(u_SunDirection));

        vec4 fog = vec4(0.75, 0.75, 0.75, 1);
        float dist = length(fs_Pos.xz - u_Player.xz) * 0.01; // fog moves with player
        
        // Compute final shaded color
        vec4 color = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        color = mix(color, fog, pow(smoothstep(0, 1, min(1, dist)), 2));

        //UNCOMMENT TO ADD FOG
        out_Col = vec4(color.rgb, diffuseColor.a);

        //UNCOMMENT TO REMOVE FOG
        //out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);

        out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        //out_Col = vec4(vec3(fs_UV.xy, 0) * lightIntensity, diffuseColor.a);
}
