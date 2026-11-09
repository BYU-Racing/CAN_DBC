import cantools

db = cantools.database.load_file("BYU_Racing_CAN.dbc")
msg = db.get_message_by_name("TireTemperature")

decoded = msg.decode(b'\x02\x27\x20\x03\x00\x20\x00\x00')
print(decoded)