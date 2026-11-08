# Mouse Lock Fix and Sensitivity Adjuster for Grand Theft Auto: Vice City
This modification addresses mouse not working in GTA: Vice City and allows the
user to freely modify their mouse sensitivity, especially vertical (y-axis)
sensitivity, which is unbalanced in the game by default.

The mod is designed for players who wish to play VC:MP 0.3z R2 with their
current 0.4 sensitivity settings, making it **fully compatible with
single-player Vice City** as well.

## Compatibility
The mod requires `gta-vc.exe` v1.0 and does not support other versions of
GTA: VC.

## Installation
1. Download
[**mousefix.zip**](https://github.com/sfwidde/gta-vc-mouse-fix/releases/latest/download/mousefix.zip)
from the
[**Releases**](https://github.com/sfwidde/gta-vc-mouse-fix/releases/latest)
page.
2. Extract `mousefix.asi` and `mousefix.txt` from the **.zip** archive to your
game's root directory.
3. Enjoy—fixes have been applied!

## How to Configure In-game Mouse Sensitivity
### `mousefix.txt` Configuration File
Each setting that we will cover below has a default value in case it is missing
or its value is invalid, as specified in the corresponding "**Default value**"
field.

- `mouse_x`
	- **Description:** Horizontal mouse sensitivity (x-axis).
	- **Acceptable values:** Any number that is at least 0.0.
	- **Default value:** 0.0
- `mouse_y`
	- **Description:** Vertical mouse sensitivity (y-axis).
	- **Acceptable values:** Any number that is at least 0.0.
	- **Default value:** 1.2

1. `mouse_x` and `mouse_y` setting values are interchangeable with those of
version 0.4 of VC:MP (`game_sensitivity` and `game_sensitivity_ratio`
respectively). For instance, a value of 0.5 for `mouse_y` will not set y-axis
sensitivity to 0.5, but will instead set it to half the value of `mouse_x`.
2. Values in the configuration file can be modified whilst in-game. Changes
shall be automatically detected upon returning to the game.
