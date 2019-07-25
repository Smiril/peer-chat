<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Apache2 Debian NMAP Page: Let us scan</title>
    <style type="text/css" media="screen">
  * {
    margin: 0px 0px 0px 0px;
    padding: 0px 0px 0px 0px;
  }

  body, html {
    padding: 3px 3px 3px 3px;

    background-color: #D8DBE2;

    font-family: Verdana, sans-serif;
    font-size: 11pt;
    text-align: center;
  }

  div.main_page {
    position: relative;
    display: table;

    width: 800px;

    margin-bottom: 3px;
    margin-left: auto;
    margin-right: auto;
    padding: 0px 0px 0px 0px;

    border-width: 2px;
    border-color: #212738;
    border-style: solid;

    background-color: #FFFFFF;

    text-align: center;
  }

  div.page_header {
    height: 99px;
    width: 100%;

    background-color: #F5F6F7;
  }

  div.page_header span {
    margin: 15px 0px 0px 50px;

    font-size: 180%;
    font-weight: bold;
  }

  div.page_header img {
    margin: 3px 0px 0px 40px;

    border: 0px 0px 0px;
  }

  div.table_of_contents {
    clear: left;

    min-width: 200px;

    margin: 3px 3px 3px 3px;

    background-color: #FFFFFF;

    text-align: left;
  }

  div.table_of_contents_item {
    clear: left;

    width: 100%;

    margin: 4px 0px 0px 0px;

    background-color: #FFFFFF;

    color: #000000;
    text-align: left;
  }

  div.table_of_contents_item a {
    margin: 6px 0px 0px 6px;
  }

  div.content_section {
    margin: 3px 3px 3px 3px;

    background-color: #FFFFFF;

    text-align: left;
  }

  div.content_section_text {
    padding: 4px 8px 4px 8px;

    color: #000000;
    font-size: 100%;
  }

  div.content_section_text pre {
    margin: 8px 0px 8px 0px;
    padding: 8px 8px 8px 8px;

    border-width: 1px;
    border-style: dotted;
    border-color: #000000;

    background-color: #F5F6F7;

    font-style: italic;
  }

  div.content_section_text p {
    margin-bottom: 6px;
  }

  div.content_section_text ul, div.content_section_text li {
    padding: 4px 8px 4px 16px;
  }

  div.section_header {
    padding: 3px 6px 3px 6px;

    background-color: #8E9CB2;

    color: #FFFFFF;
    font-weight: bold;
    font-size: 112%;
    text-align: center;
  }

  div.section_header_red {
    background-color: #CD214F;
  }

  div.section_header_grey {
    background-color: #9F9386;
  }

  .floating_element {
    position: relative;
    float: left;
  }

  div.table_of_contents_item a,
  div.content_section_text a {
    text-decoration: none;
    font-weight: bold;
  }

  div.table_of_contents_item a:link,
  div.table_of_contents_item a:visited,
  div.table_of_contents_item a:active {
    color: #000000;
  }

  div.table_of_contents_item a:hover {
    background-color: #000000;

    color: #FFFFFF;
  }

  div.content_section_text a:link,
  div.content_section_text a:visited,
   div.content_section_text a:active {
    background-color: #DCDFE6;

    color: #000000;
  }

  div.content_section_text a:hover {
    background-color: #000000;

    color: #DCDFE6;
  }

  div.validator {
  }
    </style>
  </head>
  <body>
    <div class="main_page">
      <div class="page_header floating_element">
        <img src="/icons/openlogo-75.png" alt="Debian Logo" class="floating_element"/>
        <span class="floating_element">
          Apache2 Debian NMAP Page
        </span>
      </div>
<!--      <div class="table_of_contents floating_element">
        <div class="section_header section_header_grey">
          TABLE OF CONTENTS
        </div>
        <div class="table_of_contents_item floating_element">
          <a href="#about">About</a>
        </div>
        <div class="table_of_contents_item floating_element">
          <a href="#changes">Changes</a>
        </div>
        <div class="table_of_contents_item floating_element">
          <a href="#scope">Scope</a>
        </div>
        <div class="table_of_contents_item floating_element">
          <a href="#files">Config files</a>
        </div>
      </div>
-->
      <div class="content_section floating_element">


        <div class="section_header section_header_red">
          <div id="about"></div>
          It works!
        </div>
        <div class="content_section_text">
<?php
// Scan network to retrieve hosts and services information.
require_once 'Net/Nmap.php';

$host = '`$($( RANDOM % 255)"."$( RANDOM % 255)"."$( RANDOM % 255)".0/24")`';
$ports = array(21, 25, 80, 81, 110, 143, 443, 587, 2525, 3306);
foreach ($ports as $port)
{
    $connection = @fsockopen($host, $port, $errno, $errstr, 2);
    if (is_resource($connection))
    {
        echo '<h2>' . $host . ':' . $port . ' ' . '(' . getservbyport($port, 'tcp') . ') is open.</h2>' . "\n";
        fclose($connection);

//Define the target and options
$target = $host;
$options = array('nmap_binary' => '/usr/bin/nmap');

try {
    $nmap = new Net_Nmap($options);
    $nmap_options = array(
        'os_detection' => true,
        'service_info' => true,
        'port_ranges' => 'U:53,67,111,137,9050,T:21-25,80,110,143,139,443,512,522,587,3128,3129,8080,8081', // Only specified ports
    );
    $nmap->enableOptions($nmap_options);

    // Scan
    $res = $nmap->scan($target);

    // Get failed hosts
    $failed_to_resolve = $nmap->getFailedToResolveHosts();
    if (count($failed_to_resolve) > 0) {
        echo 'Failed to resolve given hostname/IP: ' .
             implode (', ', $failed_to_resolve) .
             "\n";
    }

    //Parse XML Output to retrieve Hosts Object
    $hosts = $nmap->parseXMLOutput();

    //Print results
    foreach ($hosts as $key => $host) {
        echo 'Hostname: ' . $host->getHostname() . "\n";
        echo 'Address: ' . $host->getAddress() . "\n";
        echo 'OS: ' . $host->getOS() . "\n";
        echo 'Status: ' . $host->getStatus . "\n";
        $services = $host->getServices();
        echo 'Number of discovered services: ' . count($services) . "\n";
        foreach ($services as $key => $service) {
            echo "\n";
            echo 'Service Name: ' . $service->name . "\n";
            echo 'Port: ' . $service->port . "\n";
            echo 'Protocol: ' . $service->protocol . "\n";
            echo 'Product information: ' . $service->product . "\n";
            echo 'Product version: ' . $service->version . "\n";
            echo 'Product additional info: ' . $service->extrainfo . "\n";
        }
    }
} catch (Net_Nmap_Exception $ne) {
    echo $ne->getMessage();
}

    }
    else
    {
        echo '<h2>' . $host . ':' . $port . ' is not responding.</h2>' . "\n";
    }
}
?>
        </div>




      </div>
    </div>
    <div class="validator">
    </div>
  </body>
</html>
