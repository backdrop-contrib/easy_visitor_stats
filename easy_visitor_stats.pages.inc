<?php
/**
 * @file
 */

/**
 * Page callback for admin/reports/easy-visitor-stats/dashboard.
 */
function _easy_visitor_stats_dashboard() {
  $content = array();
  // extremely crude display for now
  $header = array('href', 'duration', 'timezone', 'referrer', 'os', 'browser', 'timestamp', 'ip');
  $data = db_query("SELECT * FROM {easy_visitor_details} ORDER BY timestamp DESC LIMIT 30")->fetchAllAssoc('did');
  $rows = array();
  $today_midnight = strtotime('today midnight');// must use today to avoid weird DST shifts
  foreach ($data as $values) {
    $values = (array) $values;
    unset($values['did']);
    $values['timestamp'] = format_date($values['timestamp']);
    $ts = $today_midnight + ($values['duration'] / 1000);
    $values['duration'] = format_date($ts, 'custom', 'i:s.v');
    $rows[] = $values;
  }
  $content['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No data yet.')
  );
  return $content;
}

/**
 * Form callback for admin/reports/easy-visitor-stats/settings.
 */
function _easy_visitor_stats_settings_form($form, $form_state) {
  $form = array();
  return $form;
}
