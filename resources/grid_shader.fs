#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Custom Uniforms we will send
uniform vec2 gridSize; // (Columns, Rows)
uniform float gap;     // 0.0 to 0.5 (Spacing)
uniform float radius;  // 0.0 to 0.5 (Roundness)

void main() {
    // 1. Calculate which Grid Cell this pixel belongs to
    vec2 cellCoord = floor(fragTexCoord * gridSize);
    vec2 cellUV = fract(fragTexCoord * gridSize); // 0.0 to 1.0 inside the cell

    // 2. Sample the color from the texture (Point filtering ensures we get the exact cell state)
    // We offset by 0.5 to sample the center of the texel
    vec4 texColor = texture(texture0, (cellCoord + 0.5) / gridSize);

    // If the cell is empty/dead (Alpha 0 or Black), just draw it as is
    if (texColor.a == 0.0) {
        finalColor = texColor;
        return;
    }

    // 3. SDF Logic for Rounded Box with Padding
    // Center the coordinates (-0.5 to 0.5)
    vec2 p = cellUV - 0.5;
    
    // Calculate the box size (0.5 is full cell, subtract gap)
    vec2 boxSize = vec2(0.5 - gap);
    
    // Signed Distance Field math for rounded box
    vec2 d = abs(p) - (boxSize - radius);
    float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;

    // 4. Output
    if (dist > 0.0) {
        // We are inside the gap/corner area -> Transparent
        finalColor = vec4(0.0); 
    } else {
        // We are inside the rounded shape -> Use the texture color (Living/Dead)
        finalColor = texColor;
    }
}

