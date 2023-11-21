#version 330

uniform ivec2 u_Dimensions;
uniform int u_Time;
uniform sampler2D u_RenderedTexture;

in vec2 fs_UV;

out vec4 color;

// Taken from CIS 5600 lecture slides. Credit to Professor Adam Mally.
vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)),
                 dot(p, vec2(269.5,183.3))))
                 * 43758.5453);
}

// Taken from CIS 5600 lecture slides. Credit to Professor Adam Mally.
float WorleyNoise(vec2 uv) {
    uv *= 15.0; // Now the space is 15x15 instead of 1x1.
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            vec2 neighbor = vec2(float(x), float(y)); // Direction in which neighbor cell lies.
            vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell.
            vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point.
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float surflet(vec2 P, vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    vec2 gradient = 2.f * random2(gridPoint) - vec2(1.f);
    // Get the vector from the grid point to P
    vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}


float perlinNoise(vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
            for(int dy = 0; dy <= 1; ++dy) {
                surfletSum = surfletSum + surflet(uv, floor(uv) + vec2(dx, dy));
            }
    }
    return surfletSum;
}



vec4 setColor() {
    float distance = WorleyNoise(fs_UV) * WorleyNoise(fs_UV) * WorleyNoise(fs_UV);
    vec2 point = random2(fs_UV);
    vec4 original_Texture = texture(u_RenderedTexture, fs_UV),
         cell_Texture = texture(u_RenderedTexture, point);

    color = vec4(0, 0, 0, 0);

    // Modify original texture color based on time, Worley noise, and the cell's color sample.
    color[0] = (original_Texture[0]) * 0.3;
    color[1] = (original_Texture[1]) * 0.3;
    color[2] = original_Texture[2] + sin(original_Texture[2] * cell_Texture[2] * (u_Time % 1000) * 0.01);
    color[3] = 1.0;

    return color;
}

void main()
{
    setColor();
}
