<?php

if( !defined( 'MEDIAWIKI' ) ) {
  echo( "This file is part of an extension to the MediaWiki software and cannot be used standalone.\n" );
  die( true );
}

$wgExtensionCredits['other'][] = array(
				       'name' => 'PatchOutput + HTMLets + BoilerPlateSelection = GenDoc',
				       'version' => '1.0',
				       'author' => 'User:RV1971, User:Dtsig, Daniel Kinzler, Brian Mingus ',
				       'url' => 'http://grey.colorado.edu',
				       'description' => 'Insert an Iframe if the page title matches an available gendoc page in /usr/local/gendoc.'
				       );

global $wgHooks;
$wgHooks['OutputPageBeforeHTML'][] = 'GendocSetOutput';

function GendocSetOutput( &$outputPage, &$text ) {
  global $wgTitle;
  $name = split('/',$wgTitle->getPrefixedText());

  if((sizeof($name) == 2)) {
    $name[0] = strtolower($name[0]);
    $name = implode('/',$name);
    $dir = "http://grey.colorado.edu/gendoc/";
    $output = "<iframe src=\"".$dir.$name.".html\" width=\"100%\" name=\"".$name."\" style=\"frameborder:0;marginwidth:0;marginheight:0;scrolling:no;\" title=\"".$dir.$name."\">[Your user agent does not support frames or is currently configured not to display frames. However, you may visit <a href=\"".$dir.$name."\">The original page</a></iframe>";
    $gendocFile =  '/usr/local/gendoc/'.$name.'.html';

    if(file_exists($gendocFile)) {
      $text =& str_replace('</p><p><br />', $output.'</p><p><br />', $text);
      $text =& str_replace('<div class="noarticletext">', $output.'<div class="noarticletext">', $text);
    }
  }
  return true;
}

/**
* Pass file content unchanged. May get mangeled by late server pass.
**/
define('HTMLETS_NO_HACK', 'none');

/**
* Normalize whitespace, apply special stripping and escaping to avoid mangeling.
* This will break pre-formated text (pre tags), and may interfere with JavaScript
* code under some circumstances.
**/
define('HTMLETS_STRIP_HACK', 'strip');

/**
* bypass late parser pass using ParserAfterTidy. 
* This will get the file content safely into the final HTML.
* There's no obvious trouble with it, but it just might interfere with other extensions.
**/
define('HTMLETS_BYPASS_HACK', 'bypass');

$wgHTMLetsHack = HTMLETS_BYPASS_HACK; #hack to use to work around bug #8997. see constant declarations.

$wgHTMLetsDirectory = NULL;

$wgExtensionFunctions[] = "wfHTMLetsExtension";

function wfHTMLetsExtension() {
    global $wgParser;
    $wgParser->setHook( "gendoc", "wfRenderHTMLet" );
}

# The callback function for converting the input text to HTML output
function wfRenderHTMLet( $name, $argv, &$parser ) {
    global $wgHTMLetsDirectory, $wgHTMLetsHack, $IP;

    #HACKs for bug 8997
    $hack = @$argv['hack'];
    if ( $hack == 'strip' ) $hack = HTMLETS_STRIP_HACK;
    else if ( $hack == 'bypass' ) $hack = HTMLETS_BYPASS_HACK;
    else if ( $hack == 'none' || $hack == 'no' ) $hack = HTMLETS_NO_HACK;
    else $hack = $wgHTMLetsHack;

    $dir = "http://grey.colorado.edu/gendoc/";
    $output = "<iframe src=\"".$dir.$name.".html\" width=\"100%\" name=\"".$name."\" style=\"frameborder:0;marginwidth:0;marginheight:0;scrolling:no;\" title=\"".$dir.$name."\">[Your user agent does not support frames or is currently configured not to display frames. However, you may visit <a href=\"".$dir.$name."\">The original page</a></iframe>";

    if ($output === false) $output = '<div class="error">Failed to load html file '.htmlspecialchars($name).'</div>';

    #HACKs for bug 8997
    if ( $hack == HTMLETS_STRIP_HACK ) {
        $output = trim( preg_replace( '![\r\n\t ]+!', ' ', $output ) ); //normalize whitespace
        $output = preg_replace( '!(.) *:!', '\1:', $output ); //strip blanks before colons

        if ( strlen($output) > 0 ) { //escape first char if it may trigger wiki formatting
            $ch = substr( $output, 0, 1);

            if ( $ch == '#' ) $output = '&#35;' . substr( $output, 1);
            else if ( $ch == '*' ) $output = '&#42;' . substr( $output, 1);
            else if ( $ch == ':' ) $output = '&#58;' . substr( $output, 1);
            else if ( $ch == ';' ) $output = '&#59;' . substr( $output, 1);
        }
    }
    else if ( $hack == HTMLETS_BYPASS_HACK ) {
        global $wgHooks;

        if ( !isset($wgHooks['ParserAfterTidy']) || !in_array('wfRenderHTMLetHackPostProcess', $wgHooks['ParserAfterTidy']) ) {
            $wgHooks['ParserAfterTidy'][] = 'wfRenderHTMLetHackPostProcess';
        }

        $output = '<!-- @HTMLetsHACK@ '.base64_encode($output).' @HTMLetsHACK@ -->';
    }

    return $output;
}

function wfRenderHTMLetHackPostProcess( &$parser, &$text ) {
   $text = preg_replace(
        '/<!-- @HTMLetsHACK@ ([0-9a-zA-Z\\+\\/]+=*) @HTMLetsHACK@ -->/esm',
        'base64_decode("$1")',
        $text
   ); 

   return true;
}


?>
