# Project Structure Recommendations for Latte-Deck

This document outlines recommended improvements to the file structure and naming conventions for the Latte-Deck project to enhance maintainability, readability, and scalability.

## Current Structure Issues

The current project structure has several limitations:

1. **Flat directory structure**: All source files are in the root directory, making navigation difficult as the project grows
2. **Inconsistent naming conventions**: Mixed naming styles (camelCase, snake_case, etc.)
3. **Ambiguous file purposes**: Some filenames don't clearly indicate their functionality
4. **No clear separation of concerns**: Hardware-specific, USB, and application logic are mixed

## Recommended Directory Structure

```
/latte-deck/
├── latte-deck.ino            # Main Arduino sketch file
├── README.md                 # Project documentation
├── LICENSE                   # License file
├── .gitignore                # Git ignore file
├── src/                      # Source code directory
│   ├── config/               # Configuration files
│   │   ├── config.h          # Main configuration header
│   │   ├── pins.h            # Pin definitions
│   │   └── usb_config.h      # USB configuration
│   ├── hid/                  # HID implementation
│   │   ├── hid_composite.h   # Composite HID interface
│   │   ├── hid_composite.cpp # Composite HID implementation
│   │   ├── hid_power.h       # Power device HID interface
│   │   ├── hid_power.cpp     # Power device HID implementation
│   │   ├── hid_keyboard.h    # Keyboard HID interface
│   │   ├── hid_keyboard.cpp  # Keyboard HID implementation
│   │   ├── hid_mouse.h       # Mouse HID interface
│   │   └── hid_mouse.cpp     # Mouse HID implementation
│   ├── input/                # Input handling
│   │   ├── gamepad.h         # Gamepad interface
│   │   └── gamepad.cpp       # Gamepad implementation
│   ├── power/                # Power management
│   │   ├── ups.h             # UPS interface
│   │   ├── ups.cpp           # UPS implementation
│   │   ├── battery.h         # Battery management interface
│   │   └── battery.cpp       # Battery management implementation
│   └── utils/                # Utility functions
│       ├── led_control.h     # LED control interface
│       └── led_control.cpp   # LED control implementation
├── include/                  # External library headers
├── lib/                      # Custom libraries
└── docs/                     # Documentation
    ├── hardware/             # Hardware documentation
    ├── software/             # Software documentation
    └── images/               # Documentation images
```

## Recommended File Naming Conventions

1. **Use snake_case for filenames**: All lowercase with underscores separating words (e.g., `hid_composite.h` instead of `compositeHID.h`)
2. **Use descriptive, specific names**: Names should clearly indicate the file's purpose
3. **Group related functionality**: Files with related functionality should have similar prefixes (e.g., `hid_*` for all HID-related files)
4. **Separate interface and implementation**: Use `.h` for interfaces and `.cpp` for implementations

## Recommended Code Organization

### Configuration

Move all configuration constants to dedicated configuration files:

- **config.h**: Main configuration options
- **pins.h**: Pin definitions and hardware connections
- **usb_config.h**: USB and HID-specific configuration

### Naming Conventions

1. **Constants**: `UPPER_SNAKE_CASE` with descriptive prefixes (e.g., `LATTE_USB_MANUFACTURER`)
2. **Functions**: `camelCase` or `snake_case` (choose one and be consistent)
3. **Classes**: `PascalCase`
4. **Namespaces**: `PascalCase`
5. **Private members**: Prefix with underscore (e.g., `_privateVariable`)

### Code Organization

1. **Separate concerns**: Each file should have a single responsibility
2. **Use namespaces**: Group related functionality in namespaces
3. **Minimize global variables**: Encapsulate state in classes or namespaces
4. **Document public interfaces**: Add comments to explain the purpose of functions and classes

## Implementation Strategy

To transition to this new structure:

1. **Create the directory structure**: Set up the folders as outlined above
2. **Move files**: Relocate existing files to their appropriate directories
3. **Refactor includes**: Update include paths in all files
4. **Rename files**: Apply the new naming conventions
5. **Update documentation**: Update README.md and other documentation to reflect the new structure

This reorganization will make the codebase more maintainable, easier to navigate, and better prepared for future enhancements.

## Benefits

1. **Improved maintainability**: Clear organization makes code easier to maintain
2. **Better scalability**: Structured approach allows for easier addition of new features
3. **Enhanced readability**: Consistent naming and organization improves code readability
4. **Easier collaboration**: Well-organized code is easier for new contributors to understand
5. **Reduced conflicts**: Separation of concerns minimizes merge conflicts in version control

By implementing these recommendations, the Latte-Deck project will be more robust, maintainable, and professional.
