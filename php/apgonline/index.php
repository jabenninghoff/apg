<?
################################################################
# APGOnline v2.1.0
################################################################
# Config data
################################################################
 
  $apg_title = "Automated Password Generator Online" ;

  $generator     = "/usr/local/bin/apg -q" ; # APG location
  #
  # Default options
  #
  $default_sl            = "y";  # Use small letters symbol set. Define "n" if not.
  $default_cl            = "y";  # Use capital letters symbol set. Define "n" if not.
  $default_nb            = "y";  # Use numeral symbol set. Define "n" if not.
  $default_ss            = "y";  # Use special symbol set. Define "n" if not.
  $default_algo          = "2";  # 1-random. 2-pronounceable.
  $default_numofpass     = "6";  # default number of passwords to generate. Up to 255.
  $default_minpasslength = "6";  # default minimum password length. Up to 255.
  $default_maxpasslength = "8";  # default maximum password length. Up to 255.
  $default_clseed        = "";   # default command line seed.

  $print_command_line = "false" ; # true|false define it to something else to
                                 # disable command line printing
  ####################################################
  # Theme settings
  #
  include ('themes/default.php');

  ####################################################
  # Language settings
  include ('lang/english.php');

################################################################
# End of Config data
################################################################
######################################################
# Print formatted text
#
function print_text($font,$size,$color,$text)
{
 print "<font ";
 if ($font != "default") print "face=\"$font\" ";
 if ($size != "0") print "size=\"$size\" ";
 print "color=\"$color\">";
 print "$text</font>";
}
#################
# Cookie analyzer
# d is delimiter
#
  if (isset($apg_online_cky) && (!$_POST['save_settings'])) {
     $tok = strtok ($apg_online_cky,"d");
     $i = 0;
     while ($tok) {
        if (($tok == "2") && ($i == "0")) $default_algo = "2";
        if (($tok == "1") && ($i == "0")) $default_algo = "1";
        if (($tok == "l") && ($i == "1")) $default_sl = "y";
	else if (($tok != "l") && ($i == "1")) $default_sl = "n";
        if (($tok == "c") && ($i == "2")) $default_cl = "y";
	else if (($tok != "c") && ($i == "2")) $default_cl = "n";
        if (($tok == "n") && ($i == "3")) $default_nb = "y";
	else if (($tok != "n") && ($i == "3")) $default_nb = "n";
        if (($tok == "s") && ($i == "4")) $default_ss = "y";
	else if (($tok != "n") && ($i == "4")) $default_ss = "n";
        if ((is_numeric($tok)) && ($i == "5")) $default_numofpass = $tok;
        if ((is_numeric($tok)) && ($i == "6")) $default_minpasslength = $tok;
        if ((is_numeric($tok)) && ($i == "7")) $default_maxpasslength = $tok;
	$i = $i + 1;
        $tok = strtok ("d");
	}
     $i = 0;
     }


###########
# Algorithm
#
if (!$_POST['algo']) $algo = $default_algo;
else $algo = $_POST['algo'];
switch ($algo)
   {
    case "1":
       $generator = $generator . " -a 1";
       $cookie_text = $cookie_text . "1d";
       break;
    case "2":
       $generator = $generator . " -a 0";
       $cookie_text = $cookie_text . "2d";
       break;
    case "":
       $algo = $default_algo;
       $cookie_text = $cookie_text . $algo . "d";
       break;
    default:
       break;
   }

############
# Symbolsets
#
  $genmode = " -M ";
  if (!$_POST['sl']) $sl = $default_sl;
  else $sl = $_POST['sl'];
  switch($sl)
     {
      case "y":
         $genmode = $genmode . "l";
         $cookie_text = $cookie_text . "ld";
         break;
      case "n":
         $cookie_text = $cookie_text . "ed";
         break;
      case "":
         $sl = $default_sl;
         $cookie_text = $cookie_text . $sl . "d";
         break;
      default:
         break;
     }
  if (!$_POST['cl']) $cl = $default_cl;
  else $cl = $_POST['cl'];
  switch($cl)
     {
      case "y":
         $genmode = $genmode . "c";
         $cookie_text = $cookie_text . "cd";
         break;
      case "n":
         $cookie_text = $cookie_text . "ed";
         break;
      case "":
         $cl = $default_cl;
         $cookie_text = $cookie_text . $cl . "d";
         break;
      default:
         break;
     }
  if (!$_POST['nb']) $nb = $default_nb;
  else $nb = $_POST['nb'];
  switch($nb)
     {
      case "y":
         $genmode = $genmode . "n";
         $cookie_text = $cookie_text . "nd";
         break;
      case "n":
         $cookie_text = $cookie_text . "ed";
         break;
      case "":
         $nb = $default_nb;
         $cookie_text = $cookie_text . $nb . "d";
         break;
      default:
         break;
     }
  if (!$_POST['ss']) $ss = $default_ss;
  else $ss = $_POST['ss'];
  switch($ss)
     {
      case "y":
         $genmode = $genmode . "s";
         $cookie_text = $cookie_text . "sd";
         break;
      case "n":
         $cookie_text = $cookie_text . "ed";
         break;
      case "":
         $ss = $default_ss;
         $cookie_text = $cookie_text . $ss . "d";
         break;
      default:
         break;
     }
  if ($genmode != " -M ")
     $generator = $generator . $genmode;

###############################
# Number of passwords parameter
#
  if (!$_POST['numofpass']) {
     $numofpass = $default_numofpass;
     $generator= $generator . " -n " . $numofpass;
     $cookie_text = $cookie_text . $numofpass . "d";
     }
  else if (is_numeric($_POST['numofpass'])) {
     $numofpass = $_POST['numofpass'];
     if ($numofpass >= "255") {
        $numofpass = "255";
        $generator= $generator . " -n " . $numofpass;
	$cookie_text = $cookie_text . $numofpass . "d";
	}         
     else {
        $generator= $generator . " -n " . $numofpass;
	$cookie_text = $cookie_text . $numofpass . "d";
	}
     }
  else {
     $numofpass = $default_numofpass;
     $generator= $generator . " -n " . $numofpass;
     $cookie_text = $cookie_text . $numofpass . "d";
     }

###################################
# Minimum password length parameter
#
  if (!$_POST['minpasslength']) {
     $minpasslength = $default_minpasslength;
     $generator= $generator . " -m " . $minpasslength;
     $cookie_text = $cookie_text . $minpasslength . "d";
     }
  else if (is_numeric($_POST['minpasslength'])) {
     $minpasslength = $_POST['minpasslength'];
     if ($minpasslength >= "255") {
        $minpasslength = "255";
        $generator= $generator . " -m " . $minpasslength;
	$cookie_text = $cookie_text . $minpasslength . "d";
	}
     else {
        $generator= $generator . " -m " . $minpasslength;
	$cookie_text = $cookie_text . $minpasslength . "d";
	}
     }
  else {
     $minpasslength = $default_minpasslength;
     $generator= $generator . " -m " . $minpasslength;
     $cookie_text = $cookie_text . $minpasslength . "d";
     }

###################################
# Maximum password length parameter
#
  if (!$_POST['maxpasslength']) {
     $maxpasslength = $default_maxpasslength;
     $generator= $generator . " -x " . $maxpasslength;
     $cookie_text = $cookie_text . $maxpasslength . "d";
     }
  else if (is_numeric($_POST['maxpasslength'])) {
     $maxpasslength = $_POST['maxpasslength'];
     if ($maxpasslength >= "255") {
        $maxpasslength = "255";
        $generator= $generator . " -x " . $maxpasslength;
	$cookie_text = $cookie_text . $maxpasslength . "d";
	}
     else {
        $generator= $generator . " -x " . $maxpasslength;
	$cookie_text = $cookie_text . $maxpasslength . "d";
	}
     }
  else {
     $maxpasslength = $default_maxpasslength;
     $generator= $generator . " -x " . $maxpasslength;
     $cookie_text = $cookie_text . $maxpasslength . "d";
     }

#############
# Random seed
#
  if (!$_POST['clseed']) {
     $clseed = $default_clseed;
     }
  else {
     $clseed = $_POST['clseed'];
     #
     # base64_encode() is used for security reasons
     #
     $generator = $generator . " -c " . base64_encode($clseed);
     }

###############
# Save settings
#
if ($_POST['save_settings'] == "s") {
   setcookie("apg_online_cky");
   setcookie("apg_online_cky", "$cookie_text");
   }
else if ($_POST['save_settings'] == "r") setcookie("apg_online_cky");

###############################################################
  print "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
  print "<html>\n";
  print "<head>\n";
  print " <meta name=\"author\" content=\"Adel I. Mirzazhanov\">\n";
  print " <title>APGOnline</title>\n";
  print "</head>\n";
  print "<body text=\"$page_text\" bgcolor=\"$page_bgcolor\" link=\"$page_link\" alink=\"$page_alink\" vlink=\"$page_vlink\">\n";
  unset ($passwords, $outpasswords);
  exec ($generator, $passwords);
  $max_ii = count($passwords);
  for ($ii = 0; $ii < $max_ii; $ii++)
      {
       $outpasswords[$ii] = htmlspecialchars($passwords[$ii] , ENT_QUOTES);
      }
  unset ($passwords);
  $ii = 0;
  print "<center>\n";
  print "<form method=\"post\" action=\"$PHP_SELF\" name=\"main\">\n";
  print "<table cellpadding=\"0\" cellspacing=\"2\" border=\"0\" width=\"$interface_width\">\n";
  print "  <tr>\n";
  print "   <td align=\"center\" colspan=\"2\" bgcolor=\"$interface_header_color\">\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"100%\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <th>";
  print_text($p_header_font_face,"0",$p_header_font_color,$apg_title);
  print "</th>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   </td>\n";
  print "  </tr>\n";
  print "  <tr>\n";
  print "   <td valign=\"top\" align=\"left\">\n";
  print "   <!-- ###ALGORITHM PANNEL BEGIN -->\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$p_width\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <th align=\"left\" bgcolor=\"$p_header_bgcolor\">";
  print_text($p_header_font_face, $p_header_font_size, $p_header_font_color,$message_algorithm);
  print "</th>\n";
  print "        <tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_pronounceable);
  print"</td>\n";
  print "         <td><input type=\"radio\" name=\"algo\" value=\"2\"";
                  if ($algo == "2") print " checked"; print"></td>\n";
  print "        </tr>\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_random);
  print "</td>\n";
  print "         <td><input type=\"radio\" name=\"algo\" value=\"1\"";
                  if ($algo == "1") print " checked"; print"></td>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   <!-- ###ALGORITHM PANNEL END -->\n";
  print "   </td>\n";
  print "   <td valign=\"top\" align=\"right\">\n";
  print "   <!-- ###SYMBOLSETS PANNEL BEGIN -->\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$p_width\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <th align=\"left\" bgcolor=\"$p_header_bgcolor\">\n";
  print_text($p_header_font_face, $p_header_font_size, $p_header_font_color,$message_symbol_sets);
  print "</th>\n";
  print "        <tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_small_lerrers);
  print "</td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_yes);
  print" </td>\n";
  print "         <td><input type=\"radio\" name=\"sl\" value=\"y\"";
                  if ($sl=="y") print " checked"; print "></td>\n";
  print "         <td><input type=\"radio\" name=\"sl\" value=\"n\"";
                  if ($sl=="n") print " checked"; print "></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_no);
  print" </td>\n";
  print "        </tr>\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_cap_letters);
  print "</td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_yes);
  print" </td>\n";
  print "         <td><input type=\"radio\" name=\"cl\" value=\"y\"";
                  if ($cl=="y") print " checked"; print "></td>\n";
  print "         <td><input type=\"radio\" name=\"cl\" value=\"n\"";
                  if ($cl=="n") print " checked"; print "></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_no);
  print" </td>\n";
  print "        </tr>\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_numbers);
  print "</td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_yes);
  print" </td>\n";
  print "         <td><input type=\"radio\" name=\"nb\" value=\"y\"";
                  if ($nb=="y") print " checked"; print "></td>\n";
  print "         <td><input type=\"radio\" name=\"nb\" value=\"n\"";
                  if ($nb=="n") print " checked"; print "></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_no);
  print" </td>\n";
  print "        </tr>\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_spec_symbols);
  print "</td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_yes);
  print" </td>\n";
  print "         <td><input type=\"radio\" name=\"ss\" value=\"y\"";
                  if ($ss=="y") print " checked"; print "></td>\n";
  print "         <td><input type=\"radio\" name=\"ss\" value=\"n\"";
                  if ($ss=="n") print " checked"; print "></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_no);
  print" </td>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   <!-- ###SYMBOLSETS PANNEL END -->\n";
  print "   </td>\n";
  print "  </tr>\n";
  print "  <tr>\n";
  print "   <td valign=\"bottom\" align=\"left\">\n";
  print "   <!-- ###AMOUNT AND SIZE PANNEL BEGIN -->\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$p_width\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <th align=\"left\" bgcolor=\"$p_header_bgcolor\">";
  print_text($p_header_font_face, $p_header_font_size, $p_header_font_color,$message_amount_length);
  print "</th>\n";
  print "        <tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_num_of_pass);
  print "</td>\n";
  print "         <td><input type=\"text\" value=\"$numofpass\" name=\"numofpass\" size=\"3\" maxlength=\"3\"></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_up_to);
  print "</td>\n";
  print "        </tr>\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_min_pass_len);
  print "</td>\n";
  print "         <td><input type=\"text\" value=\"$minpasslength\" name=\"minpasslength\" size=\"3\" maxlength=\"3\"></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_up_to);
  print "</td>\n";
  print "        </tr>\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_max_pass_len);
  print "</td>\n";
  print "         <td><input type=\"text\" value=\"$maxpasslength\" name=\"maxpasslength\" size=\"3\" maxlength=\"3\"></td>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_up_to);
  print "</td>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   <!-- ###AMOUNT AND SIZE PANNEL END -->\n";
  print "   </td>\n";
  print "   <td valign=\"bottom\" align=\"right\">\n";
  print "   <!-- ###CL_SEED PANNEL BEGIN -->\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$p_width\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <th align=\"left\" bgcolor=\"$p_header_bgcolor\">";
  print_text($p_header_font_face, $p_header_font_size, $p_header_font_color,$message_user_random_seed);
  print "</th>\n";
  print "        <tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "        <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  print "         <tr>\n";
  print "          <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$submessage_seed);
  print "</td>\n";
  print "          <td align=\"right\"><input type=\"text\" name=\"clseed\" value=\"$clseed\" size=\"8\" maxlength=\"8\"></td>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   <!-- ###CL_SEED PANNEL END -->\n";
  print "   </td>\n";
  print "  </tr>\n";
  print "  <tr>\n";
  print "   <td valign=\"top\" align=\"left\">\n";
  print "   <!-- ###CLEAR_SETTINGS PANNEL BEGIN -->\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$p_width\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_remove_saved);
  print "</td>\n";
  print "         <td><input type=\"radio\" name=\"save_settings\" value=\"r\"></td>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   <!-- ###CLEAR_SETTINGS PANNEL END -->\n";
  print "   </td>\n";
  print "   <td valign=\"top\" align=\"right\">\n";
  print "   <!-- ###SAVE_SETTINGS PANNEL BEGIN -->\n";
  print "    <table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$p_width\">\n";
  print "     <tr>\n";
  print "      <td>\n";
  print "       <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  print "        <tr>\n";
  print "         <td>";
  print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$message_save_settings);
  print "</td>\n";
  print "         <td><input type=\"radio\" name=\"save_settings\" value=\"s\"</td>\n";
  print "        </tr>\n";
  print "       </table>\n";
  print "      </td>\n";
  print "     </tr>\n";
  print "    </table>\n";
  print "   <!-- ###SAVE_SETTINGS PANNEL END -->\n";
  print "   </td>\n";
  print "  </tr>\n";
  print "  <tr>\n";
  print "   <td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"$submit_button_text\"></td>\n";
  print "  </tr>\n";
  print " </table>\n";
  print "</form>\n";
  print "<!-- ###GENERATED PASSWORDS PANNEL BEGIN -->\n";
  print "<table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$interface_width\">\n";
  print " <tr>\n";
  print "  <td>\n";
  print "   <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
  print "    <tr>\n";
  print "     <th align=\"left\" bgcolor=\"$p_header_bgcolor\">";
  print_text($p_header_font_face, $p_header_font_size, $p_header_font_color,$message_generated_pass);
  print "</th>\n";
  print "    <tr>\n";
  print "   </table>\n";
  print "  </td>\n";
  print " </tr>\n";
  print " <tr>\n";
  print "  <td>\n";
  print "   <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
  $max_i = count ($outpasswords);
  for ($i = 0; $i < $max_i; $i++)
      {
       print "    <tr>\n";
       if ($i % 2 == 0)
          print "     <td><tt>$outpasswords[$i]</tt></td>\n";
       else
          print "     <td bgcolor=\"$pass_hilight_color\"><tt>$outpasswords[$i]</tt></td>\n";
       print "    </tr>\n";
      }
  $i = 0;
  unset($outpasswords);
  print "   </table>\n";
  print "  </td>\n";
  print " </tr>\n";
  print "</table>\n";
  print "<br>\n";
  print "<!-- ###GENERATED PASSWORDS PANNEL END -->\n";
  if ($print_command_line == "true")
     {
      print "<!-- ###COMMAND LINE PANNEL BEGIN -->\n";
      print "<table cellpadding=\"$p_border_width\" cellspacing=\"0\" border=\"$p_3d_border_width\" bgcolor=\"$p_border_color\" width=\"$interface_width\">\n";
      print " <tr>\n";
      print "  <td>\n";
      print "   <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_header_bgcolor\" width=\"100%\">\n";
      print "    <tr>\n";
      print "     <th align=\"left\" bgcolor=\"$p_header_bgcolor\">";
      print_text($p_header_font_face, $p_header_font_size, $p_header_font_color,$message_command_line);
      print "</th>\n";
      print "    <tr>\n";
      print "   </table>\n";
      print "  </td>\n";
      print " </tr>\n";
      print " <tr>\n";
      print "  <td>\n";
      print "   <table cellpadding=\"1\" cellspacing=\"0\" border=\"0\" bgcolor=\"$p_body_bgcolor\" width=\"100%\">\n";
      print "    <tr>\n";
      print "     <td>";
      print_text($p_body_font_face,$p_body_font_size,$p_body_font_color,$generator);
      print "</td>\n";
      print "    </tr>\n";
      print "   </table>\n";
      print "  </td>\n";
      print " </tr>\n";
      print "</table>\n";
      print "   <!-- ###COMMAND LINE PANNEL END -->\n";
     }
  print "</center>\n";
  ####################################
  # Copyright dada
  #
  print "<font size=\"-3\" color=\"$copyright_color\">Copyright (c) 2001 Adel I. Mirzazhanov<br>\n";
  print "<a href=\"http://www.adel.nursat.kz/apg\">APG Homepage</a></font>\n";
  print "</body>\n";
  print "</html>\n";
?>
