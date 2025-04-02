import '../scss/styles.scss'

import { Modal, Toast } from 'bootstrap'
import $ from 'jquery'
import Module, { MyModule } from 'display_gen';

;(() => {
    'use strict'

    // const displayWorker = new Worker(new URL('display_worker.ts', import.meta.url), { type: 'module' })

    const blueprintModal = new Modal('#blueprint-modal')
    const blueprintErrorToast = new Toast('#blueprint-error-toast')

    const imageCanvas = document.getElementById('display-frame') as HTMLCanvasElement
    const imageCtx = imageCanvas.getContext('2d', { willReadFrequently: true }) as CanvasRenderingContext2D

    const simulationCanvas = document.getElementById('canvas') as HTMLCanvasElement

    var wasmModule: MyModule

    // Updated by wasmModule when ready
    var isWasmReady = false

    const defaultLongerImageDim = 64

    var lastBlueprintString: string
    var lastSourceImage: HTMLImageElement | null

    function fitResolutionToImage(image: HTMLImageElement, changeW: boolean, changeH: boolean) {
        let resetDims = !changeW && !changeH
        if (resetDims) {
            if (image.width > image.height) {
                $('#grid-width').val(defaultLongerImageDim)
                $('#grid-height').val(Math.round((defaultLongerImageDim * image.height) / image.width))
            } else {
                $('#grid-height').val(defaultLongerImageDim)
                $('#grid-width').val(Math.round((defaultLongerImageDim * image.width) / image.height))
            }
        } else {
            if (changeW) {
                let gridHeight = parseInt($('#grid-height').val() as string)
                $('#grid-width').val(Math.round((gridHeight * image.width) / image.height))
            } else {
                let gridWidth = parseInt($('#grid-width').val() as string)
                $('#grid-height').val(Math.round((gridWidth * image.height) / image.width))
            }
        }
    }

    function getFormattedImageData(image: HTMLImageElement, imgBrightness: number, width: number, height: number) {
        imageCanvas.width = width
        imageCanvas.height = height
        imageCtx.filter = `brightness(${imgBrightness}%)`
        imageCtx.drawImage(image, 0, 0, width, height)
        return imageCtx.getImageData(0, 0, width, height, {
            colorSpace: 'srgb'
        })
    }

    async function generateBlueprint() {
        // Disable form while generating the blueprint
        setFormBusy(true)

        try {
            // Make sure an image has been loaded
            let loadedImage = await loadFormImage(true)
            if (loadedImage != lastSourceImage) {
                fitResolutionToImage(loadedImage, false, false)
                lastSourceImage = loadedImage
            }

            // Get parameters
            let gridWidth = parseInt($('#grid-width').val() as string)
            let gridHeight = parseInt($('#grid-height').val() as string)
            let gridSpacing = parseInt($('#grid-spacing').val() as string)
            let imgBrightness = parseInt($('#image-brightness').val() as string)
            let simResolution = parseInt($('#sim-resolution').val() as string) as
                | 0
                | 1
                | 2
                | 3

            let useDithering = $('#use-dithering').is(':checked')
            let useAlpha = $('#use-alpha').is(':checked')
            let useBinary = $('#use-binary').is(':checked')

            // See factorio_ui.h for enum values
            let render_fmt = (
                (useBinary    ? 0x01 : 0x00) | 
                (useDithering ? 0x02 : 0x00) | 
                (useAlpha     ? 0x04 : 0x00)
            )

            // Generate and display the blueprint
            let imageData = getFormattedImageData(
                lastSourceImage,
                imgBrightness,
                gridWidth,
                gridHeight
            )

            // Create buffer for pixel data
            const pixelBuffer = new Uint32Array(imageData.data.buffer);
            const arrayLength = pixelBuffer.length
            const emscriptenPtr = wasmModule._Process_Init(arrayLength)

            // Assign pixel data to buffer
            const emscriptenArray = new Uint32Array(wasmModule.HEAPU32.buffer, emscriptenPtr, arrayLength)
            emscriptenArray.set(pixelBuffer);

            // Process image
            const blueprint_str_ptr = wasmModule._Process_Image(emscriptenPtr, imageData.width, imageData.height, gridSpacing, simResolution, render_fmt)

            if (blueprint_str_ptr) {
                // Save the blueprint string
                lastBlueprintString = wasmModule.UTF8ToString(blueprint_str_ptr)
            } else {
                throw Error("Failed to generate blueprint string")
            }
            
        } catch (error) {
            showErrorToast(error as string)
        } finally {
            // Allow the form to be modified again
            setFormBusy(false)
        }
    }

    function validateForm() {
        // Validate fields
        $('#generate-form').addClass('was-validated')
        // TODO: Add message if wasm is not ready
        return ($('#generate-form').get(0) as HTMLFormElement).checkValidity() && isWasmReady;
    }

    function showErrorToast(error: string) {
        $('#collapseToastErrorMsg').text(error)
        blueprintErrorToast.show()
    }

    function showBlueprintString() {
        navigator.clipboard.writeText(lastBlueprintString)
        $('#blueprint-string').val(lastBlueprintString)
        blueprintModal.show()
    }

    function setFormBusy(isBusy: boolean) {
        if (isBusy) {
            $('#generate-btn').removeClass('idle')
            $('#generate-btn-msg').text('Generating...')
        } else {
            $('#generate-btn').addClass('idle')
            $('#generate-btn-msg').text('Show blueprint')
            $('#generate-btn').prop('disabled', false)
        }
        $('#generate-fieldset').prop('disabled', isBusy)
    }

    function clearBlueprintString() {
        lastBlueprintString = ''
        $('#generate-btn-msg').text('Generate blueprint')
    }

    async function loadFormImage(skipIfImageExists: boolean): Promise<HTMLImageElement> {
        // Return existing image if necessary
        if (skipIfImageExists && lastSourceImage) {
            return lastSourceImage
        }
        let imageFiles = $('#image-file').prop('files')
        if (imageFiles.length > 0) {
            // Read data url
            return await new Promise((resolve, reject) => {
                const reader = new FileReader()
                reader.onload = (event) => {
                    resolve(event.target?.result)
                }
                reader.onerror = (error) => {
                    reject(error)
                }
                reader.readAsDataURL(imageFiles[0])
            })
                // Read image
                .then(
                    (result) =>
                        new Promise((resolve, reject) => {
                            const img = new Image()
                            img.onload = (_event) => {
                                resolve(img)
                            }
                            img.onerror = (error) => {
                                reject(error)
                            }
                            img.src = result as string
                        })
                )
        } else {
            return await Promise.reject('No image files are selected.')
        }
    }

    $(window).on('load', async (e) => {
        await Module({
            'canvas': simulationCanvas,
        }).then((module) => {
            wasmModule = module
            isWasmReady = true;
        })
    })

    // When image is added, save and configure resolution
    $('#image-file').on('change', async (_event) => {
        if ($('#image-file').prop('files').length == 0) {
            return
        }
        try {
            lastSourceImage = await loadFormImage(false)
            fitResolutionToImage(lastSourceImage, false, false)
            let imgBrightness = parseInt($('#image-brightness').val() as string)
            getFormattedImageData(
                lastSourceImage,
                imgBrightness,
                lastSourceImage.width,
                lastSourceImage.height
            )
        } catch (error) {
            lastSourceImage = null
            showErrorToast(error as string)
        }
    })

    // When width is changed and an image exists, adjust the height
    $('#grid-width').on('change', (_event) => {
        if (lastSourceImage) {
            fitResolutionToImage(lastSourceImage, false, true)
        }
    })

    // When height is changed and an image exists, adjust the width
    $('#grid-height').on('change', (_event) => {
        if (lastSourceImage) {
            fitResolutionToImage(lastSourceImage, true, false)
        }
    })

    // When fields change, rebuild blueprint simulation
    $('#generate-fieldset').on('change', async (_event) => {
        if ($('#generate-form').hasClass('was-validated')) {
            if (validateForm()) {
                await generateBlueprint()
            } else {
                clearBlueprintString()
            }
        }
    })

    // Show modal with blueprint string when requested
    $('#generate-btn').on('click', async (_event) => {
        if (lastBlueprintString) {
            showBlueprintString()
        } else if (validateForm()) {
            await generateBlueprint()
        } else {
            clearBlueprintString()
        }
    })
})()
