
rendertarget("position");
rendertarget("normal");
rendertarget("albedo");
rendertarget("depth", "DEPTH24_STENCIL8");

renderpass("forms", {
    vertex: "forms.vert",
    fragment: "forms.frag",
    targets: {
        "COLOR_ATTACHMENT0"        : "position",
        "COLOR_ATTACHMENT1"        : "normal",
        "COLOR_ATTACHMENT2"        : "albedo",
        "DEPTH_STENCIL_ATTACHMENT" : "depth",
    },
    draw: "FORMS",
    stencil: [ "ALWAYS", 1, "REPLACE" ],
});

renderpass("sky", {
    vertex: "forms.vert",
    fragment: "forms.frag",
    targets: {
        "COLOR_ATTACHMENT0"        : "position",
        "COLOR_ATTACHMENT1"        : "normal",
        "COLOR_ATTACHMENT2"        : "albedo",
        "DEPTH_STENCIL_ATTACHMENT" : "depth",
    },
    draw: "SKY",
    stencil: [ "EQUAL", 0 ],
});

rendertarget("light");

renderpass("lights", {
    fragment: "light.frag",
    targets: { "COLOR_ATTACHMENT0" : "light" },
    samplers: { "positions" : "position", "normals" : "normal" },
    draw: "LIGHTS",
    stencil: [ "GEQUAL", 1 ],
});

rendertarget("edge");

renderpass("edge", {
    fragment: "edge.frag",
    targets: { "COLOR_ATTACHMENT0" : "edge" },
    samplers: { "depth_tex" : "depth", "normal_tex" : "normal" },
});

rendertarget("image");

renderpass("combine", {
    fragment: "combine.frag",
    targets: { "COLOR_ATTACHMENT0" : "image" },
    samplers: {
        "albedo" : "albedo",
        "lights" : "light",
        "depth"  : "depth",
    },
});

rendertarget("occlusion", "RGB16F", 0.75);

rendertarget("noise", "RGBA16", 0.0);
rendertargetload("noise", "noise.png", true, false, false);

renderpass("occlusion", {
    fragment: "occlusion.frag",
    targets: { "COLOR_ATTACHMENT0" : "occlusion" },
    samplers: {
        "depth_tex"  : "depth",
        "normal_tex" : "normal",
        "noise_tex"  : "noise",
    },
    size: 0.75,
    stencil: [ "GEQUAL", 1 ],
});

rendertarget("result");

renderpass("apply", {
    fragment: "apply.frag",
    targets: { "COLOR_ATTACHMENT0" : "result" },
    samplers: {
        "image_tex"  : "image",
        "effect_tex" : "occlusion",
        "noise_tex"  : "noise",
    },
});

rendertarget("temp");

renderpass("blur_u", {
    fragment: "blur_u.frag",
    targets: { "COLOR_ATTACHMENT0" : "temp" },
    samplers: { "image_tex" : "result" },
});

rendertarget("blur");

renderpass("blur_v", {
    fragment: "blur_v.frag",
    targets: { "COLOR_ATTACHMENT0" : "blur" },
    samplers: { "image_tex" : "temp" },
});

rendertarget("antialiasing");

renderpass("antialiasing", {
    fragment: "antialiasing.frag",
    targets: { "COLOR_ATTACHMENT0": "antialiasing" },
    samplers: {
        "image_tex" : "result",
        "blur_tex"  : "blur",
        "edge_tex"  : "edge",
    },
});

rendertarget("screen");

renderpass("screen", {
    fragment : "screen.frag",
    targets  : { "COLOR_ATTACHMENT0": "screen" },
    samplers : { "image_tex": "antialiasing" },
    draw     : "SCREEN",
});
