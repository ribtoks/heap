<?php

function ProduceKey($mac)
{
   $key = substr($mac, 6, 6);
    
   $hash = hash_hmac("sha256", utf8_encode($mac), utf8_encode($key));
    
   $count = 17;
   $i = 0;
   $res = "";
   while ($count--)
   {
       $str = $hash[$i].$hash[$i + 1];
       $res .= hexdec($str);
       $i += 2;
   }
   
   return substr($res, 0, 16);
}

echo ProduceKey($_REQUEST['mac']);

?>

