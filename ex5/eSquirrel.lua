eSquirrelTCP_proto = Proto("eSquirrelTCP","eSquirrelTCP Protocol")
eSquirrelUDP_proto = Proto("eSquirrelUDP","eSquirrelUDP Protocol")

local FileStructTypes = {
	[0] = "FILE" ,
	[1] = "DIR"
}

local ProtoFields = eSquirrelTCP_proto.fields
--local ProtoFields = eSquirrelUDP_proto.fields
ProtoFields.FileRequest_Header = ProtoField.new("Header","eSquirrelUDP_proto.FileRequest_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.FileRequest_FileNameLength = ProtoField.new("FileNameLength","eSquirrelUDP_proto.FileRequest_FileNameLength","ftypes.UINT16",nil,"base.DEC")
ProtoFields.FileRequest_FileName = ProtoField.new("FileName","eSquirrelUDP_proto.FileRequest_FileNameLength","ftypes.STRING",nil,"base.NONE")

ProtoFields.FileResponse_Header = ProtoField.new("Header","eSquirrelUDP_proto.FileRequest_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.FileResponse_FileLength = ProtoField.new("FileLength","eSquirrelUDP_proto.FileRequest_FileNameLength","ftypes.UINT32",nil,"base.DEC")

ProtoFields.File_Header = ProtoField.new("FileHeader","eSquirrelUDP_proto.File_Header","ftypes.UINT32",nil,"base.DEC")
ProtoFields.File_Data = ProtoField.new("FileData","eSquirrelUDP_proto.File_Data","ftypes.BYTES",nil,"base.NONE")


ProtoFields.ListRequest_Header = ProtoField.new("Header","eSquirrelTCP_proto.ListRequest_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.ListRequest_PathLength = ProtoField.new("PathLength","eSquirrelTCP_proto.ListRequest_PathLength","ftypes.UINT16",nil,"base.DEC")
ProtoFields.ListRequest_Path = ProtoField.new("Path","eSquirrelTCP_proto.ListRequest_Path","ftypes.STRING",nil,"base.NONE")

ProtoFields.ListRequest_Header = ProtoField.new("Header","eSquirrelTCP_proto.ListRequest_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.ListRequest_PathLength = ProtoField.new("PathLength","eSquirrelTCP_proto.ListRequest_PathLength","ftypes.UINT16",nil,"base.DEC")
ProtoFields.ListRequest_Path = ProtoField.new("Path","eSquirrelTCP_proto.ListRequest_Path","ftypes.STRING",nil,"base.NONE")

ProtoFields.ListResponse_Header = ProtoField.new("Header","eSquirrelTCP_proto.ListResponse_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.ListResponse_FileCount = ProtoField.new("FileCount","eSquirrelTCP_proto.ListResponse_FileCount","ftypes.UINT16",nil,"base.DEC")

ProtoFields.FileStructs_Type = ProtoField.new("Type","eSquirrelTCP_proto.FileStructs_Type","ftypes.UINT8",FileStructTypes,"base.HEX")
ProtoFields.FileStructs_FileSize = ProtoField.new("FileSize","eSquirrelTCP_proto.FileStructs_FileSize","ftypes.UINT32",nil,"base.DEC")
ProtoFields.FileStructs_FileNameLength = ProtoField.new("FileNameLength","eSquirrelTCP_proto.FileStructs_FileNameLength","ftypes.UINT16",nil,"base.DEC")
ProtoFields.FileStructs_FileName = ProtoField.new("FileName","eSquirrelTCP_proto.FileStructs_FileName","ftypes.STRING",nil,"base.NONE")

ProtoFields.DiscoveryPacket_Header = ProtoField.new("Header","eSquirrelUDP_proto.DiscoveryPacket_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.DiscoveryPacket_ServerID = ProtoField.new("ServerID","eSquirrelUDP_proto.DiscoveryPacket_ServerID","ftypes.UINT32",nil,"base.DEC")

ProtoFields.DescoveryResponse_Header = ProtoField.new("Header","eSquirrelUDP_proto.DescoveryResponse_Header","ftypes.BYTES",nil,"base.NONE")
ProtoFields.DescoveryResponse_ServerID = ProtoField.new("ServerID","eSquirrelUDP_proto.DescoveryResponse_ServerID","ftypes.UINT32",nil,"base.DEC")
ProtoFields.DescoveryResponse_port = ProtoField.new("Port","eSquirrelUDP_proto.DescoveryResponse_port","ftypes.UINT16",nil,"base.DEC")
ProtoFields.DescoveryResponse_Validation = ProtoField.new("Validation","eSquirrelUDP_proto.DescoveryResponse_Validation","ftypes.UINT32",nil,"base.HEX")

function eSquirrelUDP_proto.dissector(buffer,pinfo,tree,len,var)

	-- Set buffer offset to the first byte of buffer
	local  buffer_offset = 0

	-- Set protocol name at the table
	pinfo.cols.protocol = "eSquirrel"

	-- Create a subtree
	local subtree_squ_proto = tree:add(eSquirrelUDP_proto,buffer(0),"eSquirrel Protocol")

	-- Add all bits by selected formatted fields:
	-- ==========================================

	local read_size = 0
	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize >= 7 then
		if buffer(buffer_offset + 0,3):uint() == 0xDEAF01 then
			wasParsed = 1
			
			read_size = 3
			subtree_squ_proto:add (ProtoFields.DiscoveryPacket_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 4
			subtree_squ_proto:add (ProtoFields.DiscoveryPacket_ServerID, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
		end
	end

	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize >= 13 then
		if buffer(buffer_offset + 0,3):uint() == 0x00DEAF02 then
			pinfo.cols.info = "DummyValid"
			wasParsed = 1
			
			read_size = 3
			subtree_squ_proto:add (ProtoFields.DescoveryResponse_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 4
			subtree_squ_proto:add (ProtoFields.DescoveryResponse_ServerID, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 2
			subtree_squ_proto:add (ProtoFields.DescoveryResponse_port, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 4
			subtree_squ_proto:add (ProtoFields.DescoveryResponse_Validation, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
		end
	end
end


function eSquirrelTCP_proto.dissector(buffer,pinfo,tree,len,var)

	-- Set buffer offset to the first byte of buffer
	local  buffer_offset = 0

	-- Set protocol name at the table
	pinfo.cols.protocol = "eSquirrel"

	-- Create a subtree
	local subtree_SkuTcp_proto = tree:add(eSquirrelTCP_proto,buffer(0),"eSquirrel Protocol")

	-- Add all bits by selected formatted fields:
	-- ==========================================

	local read_size = 0
	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize >= 5 then
		if buffer(buffer_offset + 0,3):uint() == 0x00DEAF01 then
			wasParsed = 1
			
			read_size = 3
			subtree_SkuTcp_proto:add (ProtoFields.ListRequest_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 2
			subtree_SkuTcp_proto:add (ProtoFields.ListRequest_PathLength, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = (buffer(buffer_offset - (5-3),2):uint())
			subtree_SkuTcp_proto:add (ProtoFields.ListRequest_Path, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
		end
	end
	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize >= 5 then
		if buffer(buffer_offset + 0,3):uint() == 0x00DEAF02 then
			wasParsed = 1
			
			read_size = 3
			subtree_SkuTcp_proto:add (ProtoFields.ListResponse_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 2
			subtree_SkuTcp_proto:add (ProtoFields.ListResponse_FileCount, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
		end
	end


	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize >=4 then
		if buffer(buffer_offset + 0,4):uint() == buffer:len() then
			wasParsed = 1
			
			read_size = 4
			subtree_SkuTcp_proto:add (ProtoFields.File_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = (buffer(buffer_offset - (4),2):uint())
			subtree_SkuTcp_proto:add (ProtoFields.File_Data, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			return
		end
	end	
	
	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize >= 7 then
		wasParsed = 1
		while 1 do
			wasParsed = 0
			bufferSize = (buffer:len()-buffer_offset)
			if bufferSize >= 7 then
				if FileStructTypes[buffer(buffer_offset + 0,1):uint()]~=nil then
					wasParsed = 1
					
					read_size = 1
					subtree_SkuTcp_proto:add (ProtoFields.FileStructs_Type, buffer(buffer_offset, read_size))
					buffer_offset = buffer_offset + read_size
					
					read_size = 4
					subtree_SkuTcp_proto:add (ProtoFields.FileStructs_FileSize, buffer(buffer_offset, read_size))
					buffer_offset = buffer_offset + read_size
					
					read_size = 2
					subtree_SkuTcp_proto:add (ProtoFields.FileStructs_FileNameLength, buffer(buffer_offset, read_size))
					buffer_offset = buffer_offset + read_size
					
					read_size = (buffer(buffer_offset - (7-5),2):uint())
					subtree_SkuTcp_proto:add (ProtoFields.FileStructs_FileName, buffer(buffer_offset, read_size))
					buffer_offset = buffer_offset + read_size
				end
			end
			if 0 == wasParsed then
				break
			end
		end
	end

	

	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize == 7 then
		if buffer(buffer_offset + 0,3):uint() == 0x00DEAF03 then
			wasParsed = 1
			
			read_size = 3
			subtree_SkuTcp_proto:add (ProtoFields.FileRequest_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 2
			subtree_SkuTcp_proto:add (ProtoFields.FileRequest_FileNameLength, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = (buffer(buffer_offset - (5-3),2):uint())
			subtree_SkuTcp_proto:add (ProtoFields.FileStructs_FileName, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size			
		end
	end	
	

	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize == 7 then
		if buffer(buffer_offset + 0,3):uint() == 0x00DEAF03 then
			wasParsed = 1
			
			read_size = 3
			subtree_SkuTcp_proto:add (ProtoFields.FileRequest_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 2
			subtree_SkuTcp_proto:add (ProtoFields.FileRequest_FileNameLength, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = (buffer(buffer_offset - (5-3),2):uint())
			subtree_SkuTcp_proto:add (ProtoFields.FileStructs_FileName, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size			
		end
	end	
	

	bufferSize = (buffer:len()-buffer_offset)
	if bufferSize ==7 then
		if buffer(buffer_offset + 0,3):uint() == 0x00DEAF04 then
			wasParsed = 1
			
			read_size = 3
			subtree_SkuTcp_proto:add (ProtoFields.FileResponse_Header, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size
			
			read_size = 4
			subtree_SkuTcp_proto:add (ProtoFields.FileResponse_FileLength, buffer(buffer_offset, read_size))
			buffer_offset = buffer_offset + read_size			
		end
	end

	
	
	
end





--=====================================
--=====================================


--=====================================
-- Register our protoc
--=====================================


-- Loads the table
wtap_encap_table = DissectorTable.get("wtap_encap")
encap_table = DissectorTable.get("tcp.port")
wtap_encap_table:add(wtap.USER15,eSquirrelTCP_proto)
wtap_encap_table:add(wtap.USER12,eSquirrelTCP_proto)

-- Register our protocol at the saked range
for port = 0,65535 do
	encap_table:add(port,eSquirrelTCP_proto)
end

wtap_encap_table = DissectorTable.get("wtap_encap")
encap_table = DissectorTable.get("udp.port")
wtap_encap_table:add(wtap.USER15,eSquirrelUDP_proto)
wtap_encap_table:add(wtap.USER12,eSquirrelUDP_proto)

-- Register our protocol at the saked range
for port = 1337,1338 do
	encap_table:add(port,eSquirrelUDP_proto)
end