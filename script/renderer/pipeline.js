
rendertarget("position");
rendertarget("normal");
rendertarget("diffuse");
rendertarget("specular");
rendertarget("depth", "DEPTH24_STENCIL8");

renderpass("forms", {
    vertex: "forms.vert",
    fragment: "forms.frag",
    targets: {
        "COLOR_ATTACHMENT0"        : "position",
        "COLOR_ATTACHMENT1"        : "normal",
        "COLOR_ATTACHMENT2"        : "diffuse",
        "COLOR_ATTACHMENT3"        : "specular",
        "DEPTH_STENCIL_ATTACHMENT" : "depth",
    },
    draw: "FORMS",
    depth: true,
    clear: [0, 0, 0],
    stencil: ["ALWAYS", 1, "REPLACE"],
});

renderpass("sky", {
    vertex: "forms.vert",
    fragment: "forms.frag",
    targets: {
        "COLOR_ATTACHMENT0"        : "position",
        "COLOR_ATTACHMENT1"        : "normal",
        "COLOR_ATTACHMENT2"        : "diffuse",
        "DEPTH_STENCIL_ATTACHMENT" : "depth",
    },
    fallbacks: {
        "diffuse": [1, 1, 1],
    },
    draw: "SKY",
    stencil: ["EQUAL", 0],
});

rendertarget("light");

renderpass("lights", {
    fragment: "light.frag",
    targets: {
        "COLOR_ATTACHMENT0": "light",
        "DEPTH_STENCIL_ATTACHMENT": "depth",
    },
    samplers: {
        "positions": "position",
        "normals":   "normal",
        "speculars": "specular",
    },
    fallbacks: { "light": [.5, .5, .5] },
    draw: "LIGHTS",
    clear: [0, 0, 0],
    stencil: ["LEQUAL", 1],
});

rendertarget("edge");

renderpass("edge", {
    fragment: "edge.frag",
    targets: { "COLOR_ATTACHMENT0": "edge" },
    samplers: { "depth_tex": "depth", "normal_tex": "normal" },
    fallbacks: { "edge": [0, 0, 0] },
});

rendertarget("image");

renderpass("combine", {
    fragment: "combine.frag",
    targets: { "COLOR_ATTACHMENT0": "image" },
    samplers: {
        "diffuse": "diffuse",
        "lights": "light",
        "depth": "depth",
    },
});

rendertarget("occlusion", "RGB16F", 0.75);

rendertarget("noise", "RGBA16", 0.0);
rendertargetload("noise", "noise.png", true, false, false);

renderpass("occlusion", {
    fragment: "occlusion.frag",
    targets: { "COLOR_ATTACHMENT0": "occlusion" },
    samplers: {
        "depth_tex": "depth",
        "normal_tex": "normal",
        "noise_tex": "noise",
    },
    fallbacks: { "occlusion": [1, 1, 1] },
    size: 0.75,
    clear: [0, 0, 0],
    stencil: ["LEQUAL", 1],
});

rendertarget("occluded");

renderpass("apply", {
    fragment: "apply.frag",
    targets: { "COLOR_ATTACHMENT0": "occluded" },
    samplers: {
        "image_tex":  "image",
        "effect_tex": "occlusion",
        "noise_tex":  "noise",
    },
});

rendertarget("result");

renderpass("fog", {
    fragment: "fog.frag",
    targets: {
        "COLOR_ATTACHMENT0": "result",
        "DEPTH_STENCIL_ATTACHMENT": "depth",
    },
    samplers: {
        "colors": "occluded",
        "depths": "depth",
        "lights": "light",
    },
});

rendertarget("temp");

renderpass("blur_u", {
    fragment: "blur_u.frag",
    targets: { "COLOR_ATTACHMENT0": "temp" },
    samplers: { "image_tex": "result" },
});

rendertarget("blur");

renderpass("blur_v", {
    fragment: "blur_v.frag",
    targets: { "COLOR_ATTACHMENT0": "blur" },
    samplers: { "image_tex": "temp" },
});

rendertarget("antialiasing");

renderpass("antialiasing", {
    fragment: "antialiasing.frag",
    targets: { "COLOR_ATTACHMENT0": "antialiasing" },
    samplers: {
        "image_tex": "result",
        "blur_tex": "blur",
        "edge_tex": "edge",
    },
    fallbacks: { "antialiasing": "result" },
});

rendertarget("dof");

renderpass("dof", {
    fragment: "dof.frag",
    targets: {
        "COLOR_ATTACHMENT0": "dof",
        "DEPTH_STENCIL_ATTACHMENT": "depth",
    },
    samplers: {
        "colors": "antialiasing",
        "blurs": "blur",
        "depths": "depth",
    },
});

rendertarget("tonemapped");

renderpass("tonemapping", {
    fragment: "tonemapping.frag",
    targets: { "COLOR_ATTACHMENT0": "tonemapped" },
    samplers: { "image_tex": "dof" },
    fallbacks: { "tonemapped": "dof" },
});

renderpass("screen", {
    fragment: "screen.frag",
    samplers: { "image_tex": "tonemapped" },
    draw: "SCREEN",
});

renderpass("preview", {
    fragment: "preview.frag",
    samplers: {
        "first": "normal",
        "second": "specular",
    	"third": "light",
    	"fourth": "occlusion",
    },
    draw: "SCREEN",
});
