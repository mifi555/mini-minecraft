#version 150

//Fragment shader used to create our procedural sky, including features like the sun and clouds

//view projection matrix
uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
                            // from our CPU, but it's named u_ViewProj so we don't
                            // have to bother rewriting our ShaderProgram class


uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec3 u_Eye; // Camera pos

//time variable that will be used for animation
uniform float u_Time;

//output color for each pixel
out vec4 outColor;

//constants
const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

// Sunset palette
const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0,
                               vec3(254, 192, 81) / 255.0,
                               vec3(255, 137, 103) / 255.0,
                               vec3(253, 96, 81) / 255.0,
                               vec3(57, 32, 51) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);

//**Added palettes

// Sunrise palette
const vec3 sunrise[5] = vec3[](vec3(255, 183, 107) / 255.0, // dawn colors
                               vec3(255, 214, 170) / 255.0, // early morning
                               vec3(255, 233, 221) / 255.0, // morning
                               vec3(255, 233, 221) / 255.0, // late morning
                               vec3(255, 233, 221) / 255.0); // noon

// Noon palette
const vec3 noon[5] = vec3[](vec3(64, 156, 255) / 255.0, // deep blue sky
                            vec3(135, 206, 250) / 255.0, // clear sky blue
                            vec3(176, 224, 230) / 255.0, // light sky blue
                            vec3(202, 225, 255) / 255.0, // very light blue
                            vec3(235, 246, 255) / 255.0); // almost white

// Night palette
const vec3 night[5] = vec3[](vec3(8, 32, 50) / 255.0, // deep night
                             vec3(32, 58, 90) / 255.0, // late evening
                             vec3(78, 104, 130) / 255.0, // evening
                             vec3(120, 160, 170) / 255.0, // dusk
                             vec3(144, 192, 200) / 255.0); // early dusk

//sun color
const vec3 sunColor = vec3(255, 255, 190) / 255.0;

//assignment of sunset color to determine cloud color
const vec3 cloudColor = sunset[3];

//FUNCTIONS:

//SPHERE MAPPING:
//maps 3D point on a sphere to 2D UV coordinates
vec2 sphereToUV(vec3 p) {
    float phi = atan(p.z, p.x);
    if(phi < 0) {
        phi += TWO_PI;
    }
    float theta = acos(p.y);
    return vec2(1 - phi / TWO_PI, 1 - theta / PI);
}

//COLOR MAPPING
//maps UV coordinates to specific sunset color palettes
//uses uv y coordinate to determine sunset color via interpolation (mix)
vec3 uvToSunset(vec2 uv) {
    if(uv.y < 0.5) {
        return sunset[0];
    }
    else if(uv.y < 0.55) {
        return mix(sunset[0], sunset[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(sunset[1], sunset[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(sunset[2], sunset[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(sunset[3], sunset[4], (uv.y - 0.65) / 0.1);
    }
    return sunset[4];
}

//maps UV coordinates to specific dusk color palettes based on uv.y
vec3 uvToDusk(vec2 uv) {
    if(uv.y < 0.5) {
        return dusk[0];
    }
    else if(uv.y < 0.55) {
        return mix(dusk[0], dusk[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(dusk[1], dusk[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(dusk[2], dusk[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(dusk[3], dusk[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4];
}

// Function to map UV coordinates to specific sunrise color palettes
vec3 uvToSunrise(vec2 uv) {
    if (uv.y < 0.5) {
        return sunrise[0];
    } else if (uv.y < 0.55) {
        return mix(sunrise[0], sunrise[1], (uv.y - 0.5) / 0.05);
    } else if (uv.y < 0.6) {
        return mix(sunrise[1], sunrise[2], (uv.y - 0.55) / 0.05);
    } else if (uv.y < 0.65) {
        return mix(sunrise[2], sunrise[3], (uv.y - 0.6) / 0.05);
    } else if (uv.y < 0.75) {
        return mix(sunrise[3], sunrise[4], (uv.y - 0.65) / 0.1);
    }
    return sunrise[4];
}

// Function to map UV coordinates to specific noon color palettes
vec3 uvToNoon(vec2 uv) {
    if (uv.y < 0.5) {
        return noon[0];
    } else if (uv.y < 0.55) {
        return mix(noon[0], noon[1], (uv.y - 0.5) / 0.05);
    } else if (uv.y < 0.6) {
        return mix(noon[1], noon[2], (uv.y - 0.55) / 0.05);
    } else if (uv.y < 0.65) {
        return mix(noon[2], noon[3], (uv.y - 0.6) / 0.05);
    } else if (uv.y < 0.75) {
        return mix(noon[3], noon[4], (uv.y - 0.65) / 0.1);
    }
    return noon[4];
}

// Function to map UV coordinates to specific night color palettes
vec3 uvToNight(vec2 uv) {
    if (uv.y < 0.5) {
        return night[0];
    } else if (uv.y < 0.55) {
        return mix(night[0], night[1], (uv.y - 0.5) / 0.05);
    } else if (uv.y < 0.6) {
        return mix(night[1], night[2], (uv.y - 0.55) / 0.05);
    } else if (uv.y < 0.65) {
        return mix(night[2], night[3], (uv.y - 0.6) / 0.05);
    } else if (uv.y < 0.75) {
        return mix(night[3], night[4], (uv.y - 0.65) / 0.1);
    }
    return night[4];
}

//NOISE FUNCTIONS:
//noise function that generates vec2 (2D point)
vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

//noise function that generates vec3 (3D point)
vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}

//rotation of sun on it's x axis
vec4 sunRotation(vec3 position, float angle){
    return vec4(position.x,
                cos(angle) * position.y - sin(angle) * position.z,
                sin(angle) * position.y + cos(angle) * position.z,
                0.0f);
}

//compute sun position based on time
vec3 computeSunPosition(float time) {
    time = time / 24.0;

    // Calculate the angle of the sun based on the time of day
    float angle = time * TWO_PI; // Full circle over a period of time
    // Compute sun position here (modify as needed)
    return vec3(normalize(sunRotation(normalize(vec3(0, -1.0, 0)), angle)));
}

//3D Worley noise to create cellular pattern for things like clouds
float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

//2D Worley noise
float WorleyNoise(vec2 uv)
{
    // Tile the space
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));

            // Random point inside current neighboring cell
            vec2 point = random2(uvInt + neighbor);

            // Animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

//3D Worley noise to create cellular pattern for things like clouds
float WorleyNoise3DStars(vec3 p, out vec3 color)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
                color = random3(pointInt + vec3(x, y, z));
            }
        }
    }
    return minDist;
}
//Fractal Brownian Motion Worley
float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for(int i = 0; i < 8; i++) {
        sum += WorleyNoise3D(uv * freq) * amp;
        freq *= 2;
        amp *= 0.5;
    }
    return sum;
}

//#define RAY_AS_COLOR
//#define SPHERE_UV_AS_COLOR
#define WORLEY_OFFSET
#define STARS

#define SUNSET_THRESHOLD 0.75
#define DUSK_THRESHOLD -0.1

void main()
{
    // normalized device coordinates for current pixel
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // -1 to 1 NDC

    // conversion from screen space to world space
    vec4 p = vec4(ndc.xy, 1, 1); // Pixel at the far clip plane
    p *= 1000.0; // Times far clip plane value
    p = u_ViewProj * p; // Convert from unhomogenized screen to world

    // ray direction
    vec3 rayDir = normalize(p.xyz - u_Eye);

    // uv coordinates that are based on ray's direction
    vec2 uv = sphereToUV(rayDir);

    vec2 offset = vec2(0.0);

    //Worley noise to offset UV coordinates
#ifdef WORLEY_OFFSET
    // Get a noise value in the range [-1, 1]
    // by using Worley noise as the noise basis of FBM
    offset = vec2(worleyFBM(rayDir));
    offset *= 2.0;
    offset -= vec2(1.0);
#endif
//    vec3 sunsetColor = uvToSunset(uv + offset * 0.1);

//    vec3 duskColor = uvToDusk(uv + offset * 0.1);

    //u_time already is in ms
    float timeOfDay = mod(u_Time * 0.01, 24.0);
    // Choose color palette based on time of day
    vec3 currentSkyColor;

    //hard code time intervals
    if(timeOfDay < 6.0) { // Night to Sunrise 0am - 6am
        currentSkyColor = mix(uvToNight(uv + offset * 0.1), uvToSunrise(uv + offset * 0.1), timeOfDay / 6.0);
    } else if(timeOfDay < 9.0) { // Sunrise to Noon blend 6am-9am
        currentSkyColor = mix(uvToSunrise(uv + offset * 0.1), uvToNoon(uv + offset * 0.1), (timeOfDay - 6.0) / 3.0);
    } else if(timeOfDay < 16.0) {
        currentSkyColor = uvToNoon(uv + offset * 0.1);
    } else if(timeOfDay < 18.0) { // Noon to Sunset blend 4pm - 6pm
        currentSkyColor = mix(uvToNoon(uv + offset * 0.1), uvToSunset(uv + offset * 0.1), (timeOfDay - 16.0) / 2.0);
    } else if(timeOfDay < 20.0) { // Sunset to Dusk
        currentSkyColor = mix(uvToSunset(uv + offset * 0.1), uvToDusk(uv + offset * 0.1), (timeOfDay - 18.0) / 2.0);
    } else if (timeOfDay < 24.0){ // Dusk to Night
        currentSkyColor = mix(uvToDusk(uv + offset * 0.1), uvToNight(uv + offset * 0.1), (timeOfDay - 20.0) / 4.0);
    }

#ifdef STARS
    bool isNightTime = (timeOfDay >= 18.0 || timeOfDay < 6.0);
    float starIntensity;

    if(isNightTime){
        if (timeOfDay >= 22.0 || timeOfDay < 1.0) {
        // Full night time: stars at full intensity
            starIntensity = 1.0;
        } else if (timeOfDay >= 18.0 && timeOfDay < 22.0) {
            // Transition from sunset to night
            starIntensity = smoothstep(18.0, 22.0, timeOfDay);
        } else if (timeOfDay >= 0.0 && timeOfDay < 2.0) {
            // Transition from night to sunrise
            starIntensity = 1 -smoothstep(1.0, 2.0, timeOfDay);
        } else {
            // Day time: no stars
            starIntensity = 0.0;
        }
    }

    if(isNightTime){
        vec3 starColor;
        float starDensity = WorleyNoise3DStars(rayDir * 50, starColor);

        if (starDensity < 0.1) {
        currentSkyColor = mix(currentSkyColor, starColor, starIntensity);
        }
    }
#endif
    // Use the current sky color for rendering
    outColor = vec4(currentSkyColor, 1.0);

//    vec3 sunDir = normalize(vec3(sunRotation(normalize(vec3(0, 0, -1.f)), timeOfDay * 0.15)));
    vec3 sunDir = normalize(computeSunPosition(timeOfDay));

    float sunSize = 30;
    float angle = acos(dot(rayDir, sunDir)) * 360.0 / PI;

    // If the angle between our ray dir and vector to center of sun
    // is less than the threshold, then we're looking at the sun
    if(angle < sunSize) {
        // Full center of sun
        if(angle < 7.5) {
            outColor = vec4(sunColor, 1);
        }
        // Corona of sun, mix with sky color
        else {
            //(angle - minimum)/range
            outColor = vec4(mix(sunColor, currentSkyColor, (angle - 7.5) / 22.5), 1.);
        }
    }
    // Otherwise our ray is looking into just the sky
    else {
        //dot ray with sun vector
        float raySunDot = dot(rayDir, sunDir);
        if(raySunDot > 0.75) {
            // Do nothing, sky is already correct color
        }
        // Any dot product between 0.75 and -0.1 is a LERP b/t sunset and dusk color
        else if(raySunDot > -0.1) {
            // divide by range to map from 0 to 1 and use t value for mix
            float t = (raySunDot - 0.75) / (-0.1 - 0.75);
            //use mix to blend between sun color and sky color
            //change duskColor to currentSkyColor
            outColor = vec4(mix(outColor.rgb, currentSkyColor, t), 1.);
        }
        // Any dot product <= -0.1 are pure dusk color
        else {
            //change duskColor to currentSkyColor
            outColor = vec4(currentSkyColor, 1.);
        }
    }
}
