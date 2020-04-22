greenhouse_protocol = Proto("UDP-lite-GH",  "UDP lite GreenHouse Protocol")

message_length = ProtoField.int32("greenhouse.message_length", "messageLength", base.DEC)

date = ProtoField.absolute_time("greenhouse.date", "date", base.LOCAL)

values = {}

sensor_type = ProtoField.bytes("greenhouse.sensor_type", "type", base.SPACE)


device_nr = ProtoField.bytes("greenhouse.device_number", "deviceNumber", base.DOT)

greenhouse_protocol.fields = { message_length, date, sensor_type, device_nr }


function greenhouse_protocol.dissector(buffer, pinfo, tree)
  length = buffer:len()
  if length == 0 then return end

  pinfo.cols.protocol = greenhouse_protocol.name

  local subtree = tree:add(greenhouse_protocol, buffer(), "GreenHouse Protocol Data")

  subtree:add_le(date, buffer(0,8))

  --subtree:add_le(sensor_type, buffer(8,1))
  --subtree:add_le(sensor_type, buffer(8,1):bitfield(1,1))
  subtree:add_le("sensor type: " ..buffer(8,1):bitfield(0,2))
 
   
  subtree:add_le(device_nr, buffer(9,1))

end

local tcp_port = DissectorTable.get("udp.port")
tcp_port:add(8080, greenhouse_protocol)
