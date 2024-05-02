#version 300 es

layout(location = 0) in vec3 mean;
layout(location = 1) in vec3 color;
layout(location = 2) in mat3 cov;
layout(location = 5) in float opacity;

uniform mat4 mproj;
uniform mat4 mview;

out vec3  xk;       // Center of gaussian in NDC space
out mat2  oinv_V_hat;   // 2D Covariance
out vec3  ocolor;   // Color   (straight from vertex attribute)
out float oopacity; // Opacity (straight from vertex attribute)

mat3 compute_jacobian(vec4 t) {
    // "Jacobian" refer to the first derivative in the taylor series
    // for the projective transformation (Transformation from Camera Space to NDC - (P @ t) / (P @ t)_w ) ).

    // Instead of Eq 29 in EWA splatting, I approximate the Jacobian based on the projection matrix.
    mat4 m = transpose(mproj);
    float m0t = dot(m[0], t);
    float m1t = dot(m[1], t);
    float m2t = dot(m[2], t);
    float m3t = dot(m[3], t);
    float m3tsq = m3t * m3t;

    float j00 = m[0][0] / m3t - m[3][0] * m0t / m3tsq;
    float j10 = m[1][0] / m3t - m[3][0] * m1t / m3tsq;
    float j20 = m[2][0] / m3t - m[3][0] * m2t / m3tsq;
    float j01 = m[0][1] / m3t - m[3][1] * m0t / m3tsq;
    float j11 = m[1][1] / m3t - m[3][1] * m1t / m3tsq;
    float j21 = m[2][1] / m3t - m[3][1] * m2t / m3tsq;
    float j02 = m[0][2] / m3t - m[3][2] * m0t / m3tsq;
    float j12 = m[1][2] / m3t - m[3][2] * m1t / m3tsq;
    float j22 = m[2][2] / m3t - m[3][2] * m2t / m3tsq;

    mat3 J = mat3(j00, j10, j20, j01, j11, j21, j02, j12, j22);

    return J;
}

// TODO: Futur optimization:
// * only use the fragment shader where the opacity of the gaussian is present by computing a "bounding box" and converting it in ndc coordinates;
// * Discard gaussian that are outside the frustum. (By implementing the above optimization, we should have roughly the same benefit)
void main() {

    //
    // 1. Compute u (mean) into Camera space.
    //
    vec4 tk = mview * vec4(mean,1.f); // Word to Camera Space

    vec4 x_clip = mproj * tk; // Camera to Clip Space
    if (x_clip.w < 0.) { // Cull splat that are behind the camera
        gl_Position = vec4(0.,0.,0.,1.);
        return;
    }

    xk = x_clip.xyz / x_clip.w; // Clip to NDC Space
    if (xk.x < -1.2 || xk.x > 1.2 || xk.y < -1.2 || xk.y > 1.2) {
        gl_Position = vec4(0.,0.,0.,1.);
        return;
    }

    //
    // 2. Compute Jacobian and V_hat as of eq. (31) in EWA splatting paper.
    //
    // BUG: Gaussian are properly colorized and sized, but they seems to be oriented in the wrong direction...
    // Current guess is that the View / J Matrix is not correctly applied.
    // Gaussian seems to be rotated by roughly ~90* degree in their respective space.

    mat3 J = compute_jacobian(tk);
    mat3 W = mat3(mview);
    mat3 V = cov;
    mat3 T = J * W;
    mat2 V_hat = mat2(T * V * transpose(T));

    // Multiply our kernel by another one to anti-alias the result.
    V_hat[0][0] += 0.00000001f;
    V_hat[1][1] += 0.00000001f;

    //
    // 3. Compute a bounding box that keep most of the visible information inside.
    //

    // Use 3blue1brown trick to compute the eigenvalues of a 2x2 matrix.
    float p = determinant(V_hat);
    float m = 0.5 * (V_hat[0][0] + V_hat[1][1]);

    // Eigenvalues ~ prop ~ Radius of most information
    float lambda1 = m + sqrt( m * m - p );
    float lambda2 = m - sqrt( m * m - p );

    // Take the biggest value and take it as a simple radius (sqrt(lambda1), sqrt(lambda2)) for computing the bounding box.
    const float gres = 2.f; // Higher make it more accurate, but make computation much slower.

    float radius = gres * sqrt(max(lambda1, lambda2));

    vec2 bb_corner = xk.xy + radius * (vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) - 1.) ;

    // Generate all the corners of a quad with vertices at (+-1,+-1)
    gl_Position = vec4(bb_corner, 0, 1);

    ocolor = color;
    oinv_V_hat = inverse(V_hat);
    oopacity = opacity;
}