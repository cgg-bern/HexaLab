THREE.AOPre = {

    vertexShader: [

        "varying vec3 vNorm;",

        "void main() {",
            "gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);",
            "vNorm = (modelViewMatrix * vec4(position, 1.0)).rgb;",
        "}"

    ].join( "\n" ),

    fragmentShader: [

        "varying vec3 vNorm;",

        "void main() {",
            "gl_FragColor = vec4(vNorm, 1.0);",
            // "gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);",
            // "gl_FragColor.r = gl_FragCoord.z;",
        "}"

    ].join( "\n" )

};
