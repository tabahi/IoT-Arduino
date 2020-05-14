#define AES_DEBUG 0



char data[33];


String encrypt_data(String dataString)
{
  int datalen = dataString.length();
  if (datalen > 0)
  {
    data_copy_string(dataString);
#if AES_DEBUG
    Serial.println();
    Serial.print("InE:");
    Serial.println(data);
    Serial.println("B");
#endif

    aes128_cbc_enc(key, iv, data, datalen);

    xs = "";
    for (int elen = 0; elen < datalen; elen++ )
    {
      char nchar = data[elen] & 0xFF;
      if (nchar < 0x10)
      {
        xs += "0";
      }
      xs += String(nchar, HEX);
    }
#if AES_DEBUG
    Serial.print(F("Encrypted:"));
    Serial.println(xs);
#endif

    return xs;
  }
  else
  {
    return "";
  }
}

String decrypt_data(String str)
{
  data_empty();
  hexToBytes(str, data);
  int enclen = (str.length() / 2);

  if (enclen > 0)
  {
#if AES_DEBUG
    Serial.println();
    Serial.print("InD:");
    //Serial.println(str);
    for (int itr = 0; itr < enclen; itr++ )
      Serial.print(data[itr] & 0xFF, HEX);
    Serial.println();
#endif

    aes128_cbc_dec(key, iv, data, enclen);
    xs = "";
    xs += data;
#if AES_DEBUG
    Serial.print(F("Decrypted:"));
    Serial.println(xs);
#endif

    return xs;
  }
  else
  {
    return "";
  }
}


void hexToBytes(String str, char* datax)
{
  int len = str.length();
  if (len > 1)
  {
    for (int i = 0; i < len; i += 2)
    {
      datax[i / 2] = getVal(str[i + 1]) + (getVal(str[i]) << 4);
    }
  }
}


byte getVal(char c)
{
  if (c >= '0' && c <= '9')
    return (byte)(c - '0');
  else
    return (byte)(c - 'a' + 10);
}


void data_empty()
{
  data[32] = '\0';
  for (int xu = 0; xu < 33; xu++)
  {
    data[xu] = 0x00;
  }
}

void data_copy_string(String tocopy)
{
  data_empty();
  //free(data);
  int lenCopy = tocopy.length();
  for (int xu = 0; xu < lenCopy; xu++)
  {
    data[xu] = tocopy[xu];
  }
}



