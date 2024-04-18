#version 300 es

precision highp float;
#define M_PI 3.1415926535897932384626433832795

in mat2 oinv_V_hat;
in vec3 ocolor;
in float oopacity;
in vec3 xk;

uniform vec2 uresolution;

out vec4 frag_color;

// Gaussian as used in "3D Gaussian Splatting for Real-Time Radiance Field Rendering". **Not Normalized**
float gaussian(mat2 inv_V, vec2 x) {
    return exp(-.5 * dot(x, inv_V * x));
}

void main() {

    // Compute the NDC coordinates.
    vec2 uv =  (2. * gl_FragCoord.xy) / uresolution - 1.;

    // Compute the alpha component and multiply the gaussian value by the opacity.
    float alpha = min(0.99f, oopacity * gaussian(oinv_V_hat, uv - xk.xy));

    if (alpha < 1./255.f) {
        discard;
    }

    // Compute the fragment color.
    frag_color = vec4(alpha * ocolor, alpha);
}