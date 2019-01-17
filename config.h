#pragma once
/** config.h - Global options for performance tuning */

// Starfields are usually generated dynamically each frame using a deterministic
// pseudorandom generator. If this is set to 1, the starfield background is instead
// generated and saved in a couple of images at the start of the game that are then
// composited into the game frame.
#ifndef CONFIG_PRERENDER_STARFIELD
#   define CONFIG_PRERENDER_STARFIELD 0
#endif

// The memset provided by Circle is a trivial implementation that sets a single byte
// at a time. Setting CONFIG_OWN_MEMSET will use our own implementation that will
// set 8 bytes at a time when possible (the count is larger than 8 and aligned with
// 64 bit integers.)
#ifndef CONFIG_OWN_MEMSET
#   define CONFIG_OWN_MEMSET 1
#endif

// Set CONFIG_USE_ITEM_POOL to 0 use the standard memory allocator for link list
// items. When set to 1, freed items will be added to a pool of items to be reused
// instead of deallocating the memory.
#ifndef CONFIG_USE_ITEM_POOL
#   define CONFIG_USE_ITEM_POOL 1
#endif

// Set CONFIG_GPU_PAGE_FLIPPING to 1 to allocate both the active and visible
// frame buffers in GPU memory. The GPU will be used to page between them.
// If set to 0 rendering will be to a temporary buffer in CPU ram and
// page flipping will be implemented by memcpy-ing the contents to GPU ram.
// Due to CPU memory being cached and GPU not, the latter is actually
// around 3 times faster than the former despite the final memcpy.
#ifndef CONFIG_GPU_PAGE_FLIPPING
#   define CONFIG_GPU_PAGE_FLIPPING 0
#endif

// If CONFIG_DMA_FRAME_COPY is set (and CONFIG_GPU_PAGE_FLIPPING is not set)
// the final memory copy from CPU to GPU ram is done using a DMA transfer instead
// of using the CPU. This is approximately 5 times slower thant the CPU methhod,
// but further refinements could be done to allow non-rendering code to run
// in parallel with the copying.
#ifndef CONFIG_DMA_FRAME_COPY
#   define CONFIG_DMA_FRAME_COPY 0
#endif

// CONFIG_DMA_PARALLELL will allow the main loop to start running non-rendering updates
// while the DMA operation is still copying frame data to the GPU.
// It will not block until just before starting rendering the next frame.
#ifndef CONFIG_DMA_PARALLEL
#   define CONFIG_DMA_PARALLEL 0//CONFIG_DMA_FRAME_COPY
#endif

// If set to 1, the ScreenManager class will use neon intrinsics to copy 16 pixels
// at a time to the destination using NEON intrinsics. This is currently disabled,
// as it seems to have a neglible effect.
#ifndef CONFIG_NEON_RENDER
#   define CONFIG_NEON_RENDER 1
#endif

// When set to 1, this will use the Circle framebuffer WaitForVerticalSync method
// instead of the custom interrupt handler. Should only be used for testing purposes
// as it will reduce input and network responsiveness.
#ifndef CONFIG_OLD_VSYNC
#   define CONFIG_OLD_VSYNC 0
#endif

// Sanity checks
#if CONFIG_GPU_PAGE_FLIPPING && CONFIG_DMA_FRAME_COPY
#   error "CONFIG_GPU_PAGE_FLIPPING and CONFIG_DMA_FRAME_COPY are mutually exclusive"
#endif

#if CONFIG_NEON_RENDER && CONFIG_DMA_FRAME_COPY
#   error "CONFIG_NEON_RENDER and CONFIG_DMA_FRAME_COPY are currently not compatible with each other"
#endif

#if CONFIG_DMA_PARALLEL && !CONFIG_DMA_FRAME_COPY
#   error "CONFIG_DMA_PARALLEL requires CONFIG_DMA_FRAME_COPY"
#endif
