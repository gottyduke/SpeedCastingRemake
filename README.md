# Speed Casting Remake

## Build Dependencies
* [CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE)
* [DKUtil](https://github.com/gottyduke/DKUtil)

## End User Dependencies
* [SKSE64](https://skse.silverlock.org/)
* [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444)  

## Settings
Setting | Type | Default | Description
--- | --- | --- | ---
`SpeedCastFactor` | `float` | 0.5 | Multiplier of original casting speed
`UseHook` | `bool` | true | Use hook or postload scan method to achieve this
`UseGlobal` | `bool` | false | Use value of global variable as factor
`GlobalName` | `string` | SpeedCastGlobal | Name of the global variable to use