--[[
    How To setup guide:
    To use this dzVents-event you should use at least a domoticz v4.9xxx
    First you need to rename the subsensors of roode to the names you see in the script. 
    Domoticz is not able to pass the subsensor names correctly so this is a manual step.
    1. Go to Setup -> Hardware -> Devices
    2. Search for MySensors
    3. Make sure you have exactly one person inside the room (counter is set to 1)
    4. Rename:
        4.1. "Text"-Sensor witht the value 1 to "PeopleCounter"
        4.2. "AC"-Sensor to any Room Name
        4.4. "Text"-Sensor witht the higher threshold value to "Threshold"
    5. Add all these devices
    6. Go to Setup -> Hardware
    7. Add "Dummy"-Hardware named Roode
    8. Add two "Light/Switch"-devices named "ChangeCounter" and "Recalibrated"
    9. Change ChangeCounter to a selector switch with 4 levels (Off 1,2,3,4) or as many people you want to set
    10. Change Recalibrate to a "Push on Button" device
        10.1 Set the "On Action": http://<IP:Port>/json.htm?type=command&param=udevice&idx=<ID of Threhsold-device>&nvalue=0&svalue=recalibrate 
    11. Check that the names are matching the variable names of the script below
--]]
local ChangeCounter = 'ChangeCounter'
local lastCounter 
return {
    active = true,
	logging = {
		level = domoticz.LOG_DEBUG, -- Select one of LOG_DEBUG, LOG_INFO, LOG_ERROR, LOG_FORCE to override system log level
		marker = "ChangeCounterUpdate"
	},
	on = {
		devices = {
			'ChangeCounter',
			'PeopleCounter'
		}
	},
	execute = function(domoticz, device)
	    domoticz.log("counter changed to: " .. device.state)
	    
	if (device.name == 'ChangeCounter') then
        if(device.state == "Off") then
            domoticz.devices('Room').switchOff()
            domoticz.devices('PeopleCounter').updateText("0").silent()
        elseif(tonumber(device.state) > 0) then
            domoticz.devices('Room').switchOn()
            domoticz.devices('PeopleCounter').updateText(device.state).silent()
        end
    elseif (device.name == 'PeopleCounter') then
            if tonumber(device.state) <= 4 and tonumber(device.state) > 0 then
                domoticz.devices('ChangeCounter').switchSelector(tonumber(device.state.."0")).silent()
            elseif tonumber(device.state) == 0 then
                domoticz.devices('ChangeCounter').switchSelector(0).silent()
            end
        end
	end
}