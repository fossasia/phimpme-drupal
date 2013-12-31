<?php

class XCalFormatView extends RESTServerView {
  const DATE_FORMAT = 'Ymd\This';

  function render() {
    $ical = isset($this->arguments['transform']) && $this->arguments['transform']=='ical';
    if ($ical) {
      return $this->render_ical();
    }

    $doc = new DomDocument('1.0', 'utf-8');

    $icalendar = $doc->appendChild($doc->createElement('iCalendar'));
    $icalendar->setAttribute('xmlns', 'http://www.ietf.org/rfc/rfc2445');

    $vc = $icalendar->appendChild($doc->createElement('vcalendar'));
    $this->value($doc, $vc, 'version', '2.0');
    $this->value($doc, $vc, 'prodid', $this->prodid());

    foreach ($this->model as $item) {
      $vevent = $vc->appendChild($doc->createElement('vevent'));
      $this->value($doc, $vevent, 'summary', $item->getName());
      $this->value($doc, $vevent, 'dtstart', date(self::DATE_FORMAT, $item->getStarts()));
      $this->value($doc, $vevent, 'dtend', date(self::DATE_FORMAT, $item->getEnds()));
      $this->value($doc, $vevent, 'description', $item->getDescription());
      $this->value($doc, $vevent, 'url', $item->getUrl());
    }

    $doc->formatOutput = TRUE;
    return $doc->saveXml();
  }

  private function prodid() {
    global $language;
    return '-//Drupal//' . $language->language;
  }

  private function value($doc, $p, $name, $value) {
    $elem = $p->appendChild($doc->createElement($name));
    $elem->appendChild($doc->createTextNode($value));
  }

  private function render_ical() {
    $out = "BEGIN:VCALENDAR\n";
    $out .= "VERSION:2.0\n";
    $out .= $this->ical_value('PRODID:', $this->prodid());
    foreach ($this->model as $item) {
      $out .= "BEGIN:VEVENT\n";
      $out .= $this->ical_value('SUMMARY:', $item->getName());
      $out .= $this->ical_value('DTSTART:', date(self::DATE_FORMAT, $item->getStarts()));
      $out .= $this->ical_value('DTEND:', date(self::DATE_FORMAT, $item->getEnds()));
      $out .= $this->ical_value('DESCRIPTION:', $item->getDescription());
      $out .= $this->ical_value('URL:', $item->getUrl());
      $out .= "END:VEVENT\n";
    }
    $out .= "END:VCALENDAR\n";
    return $out;
  }

  private function ical_value($preamble, $val) {
    return $preamble . $this->ical_split($preamble, $val) . "\n";
  }

  private function ical_split($preamble, $value) {
    $value = trim($value);
    $value = strip_tags($value);
    $value = preg_replace('/\n+/', ' ', $value);
    $value = preg_replace('/\s{2,}/', ' ', $value);

    $preamble_len = drupal_strlen($preamble);

    $lines = array();
    while (drupal_strlen($value)>(75-$preamble_len)) {
      $space = (75-$preamble_len);
      $mbcc = $space;
      while ($mbcc) {
        $line = mb_substr($value, 0, $mbcc);
        $oct = drupal_strlen($line);
        if ($oct > $space) {
          $mbcc -= $oct-$space;
        }
        else {
          $lines[] = $line;
          $preamble_len = 1; // Still take the tab into account
          $value = mb_substr($value, $mbcc);
          break;
        }
      }
    }
    if (!empty($value)) {
      $lines[] = $value;
    }

    return join($lines, "\n ");
  }
}