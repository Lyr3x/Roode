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