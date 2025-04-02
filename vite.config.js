import { defineConfig } from 'vite'

export default defineConfig({
    root: 'src',
    publicDir: '../public',
    build: {
        minify: true,
        outDir: '../dist',
        emptyOutDir: true
    },
    resolve: {
        alias: {
            "display_gen": "/../build/display_gen.js"
        },
    },
    server: {
        port: 8080,
        hot: true
    },
    base: '/vidtorio/'
})
