greenhouse_protocol = Proto("UDP-lite-GH",  "UDP lite GreenHouse Protocol")

message_length = ProtoField.int32("greenhouse.message_length", "messageLength", base.DEC)

date = ProtoField.absolute_time("greenhouse.date", "date")

measurement = ProtoField.float("greenhouse.date", "measurement")


greenhouse_protocol.fields = { message_length, date, measurement}

function greenhouse_protocol.dissector(buffer, pinfo, tree)
  length = buffer:len()
  if length == 0 then return end

  pinfo.cols.protocol = greenhouse_protocol.name

  local subtree = tree:add(greenhouse_protocol, buffer(), "GreenHouse Protocol Data")

  subtree:add_le(date, buffer(0,8))

  
  local subsubtree = subtree:add(greenhouse_protocol, buffer(8, 1), "sensor info (type and number)")

  subsubtree:add_le("sensor type : " ..buffer(8,1):bitfield(0,2))
  subsubtree:add_le("device number : " ..buffer(8,1):bitfield(2,5))


  local datagram_type = buffer(8,1):bitfield(0,2)

  if datagram_type < 0 or datagram_type >4 then
	--if type is invalid 
	data_length = 0 
  else
	data_length = 4
  end

  subtree:add_le(measurement, buffer(9, data_length))

end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(8080, greenhouse_protocol)
