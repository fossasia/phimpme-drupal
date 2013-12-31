<?php
/**
 *  Bencode snippet from http://paste.lisp.org/display/17178
 */

function bencode($element) { 
   $out = ""; 
   if (is_numeric($element) && $element == intval($element)) {
      $out = 'i'.$element.'e'; 
   } else if (is_string($element)) { 
      $out = strlen($element).':'.$element; 
   } else if (is_array($element) || (is_object($element) && $element=get_object_vars($element))) { 
      ksort($element); 
      if (is_string(key($element))) { 
         $out ='d'; 
         foreach($element as $key => $val) 
            $out .= bencode($key).bencode($val); 
         $out .= 'e'; 
      } else { 
         $out ='l'; 
         foreach($element as $val) 
            $out .= bencode($val); 
         $out .= 'e'; 
      } 
   }
   return $out; 
}