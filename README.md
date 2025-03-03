# Mouse lock fix and sensitivity adjuster for Grand Theft Auto: Vice City
This modification fixes mouse failing to work in GTA: Vice City on modern
hardware and allows user to freely modify their mouse sensitivity, especially
vertical (y-axis) sensitivity, which is unbalanced in the game by default.

Please bear in mind that this mod was especially made for Vice City: Multiplayer
(VC:MP) 0.4 players who wish to play VC:MP 0.3z R2 (or earlier versions) with
their current 0.4 sensitivity settings. This doesn't mean, however, that the mod
is limited to be used by VC:MP players only.

## Compatibility
This mod is compatible with
- Grand Theft Auto: Vice City (version 1.0)
- Vice City: Multiplayer (up to version 0.3z R2)

*No other versions are supported.*

## Installation steps
- Download the mod (`mousefix.zip`) from the [**Releases** page](https://github.com/sfwidde/gta-vc-mouse-fix/releases/latest).
- Extract **.zip** file contents (`mousefix.asi` and `mousefix.txt`) to your
GTA's root directory.

## Configure in-game mouse sensitivity
### `mousefix.txt` configuration file
Each setting that we will cover below has a default value in case it is missing
or its value is invalid (e.g.: `reload_key` is not present in the configuration
file by default but it implicitly defaults to the F9 key).

- `mouse_x`
	- **Description:** Horizontal mouse sensitivity (x-axis).
	- **Acceptable values:** Any floating-point greater than or equal to 0.0.
	- **Default value:** 0.0
- `mouse_y`
	- **Description:** Vertical mouse sensitivity (y-axis).
	- **Acceptable values:** Any floating-point greater than or equal to 0.0.
	- **Default value:** 1.2
- `reload_key`
	- **Description:** When changes are made to the configuration file while the
	game is running, press this key to reflect changes in-game. This way, you
	don't have to quit the game every time you need to tweak your sensitivity
	settings.
	- **Acceptable values:** Any of those on [this list](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
	(except `VK_F10`).
	- **Default value:** 0x78 (`VK_F9`)

As mentioned above, `mouse_x` and `mouse_y` setting values are interchangeable
with those of version 0.4 of VC:MP (`game_sensitivity` and
`game_sensitivity_ratio` respectively). For instance, a value of 0.5 for
`mouse_y` will not set y-axis sensitivity to 0.5 but will instead set it to half
the value of `mouse_x`.

Values in the configuration file can be modified whilst in-game. Hit
`reload_key` (default: F9) key to apply changes.