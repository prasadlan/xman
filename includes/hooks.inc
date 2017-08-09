<?php
/**
 * @file
 * REDCap hooks implementation file.
 */

require_once 'xman.inc';

/**
 * Invokes redcap_add_edit_records_page hooks.
 * @see REDCap Hooks documentation
 */
function redcap_add_edit_records_page($project_id, $instrument, $event_id) {
    xman_hook_invoke('add_edit_records_page', $project_id, array($project_id, $instrument, $event_id));
}

/**
 * Invokes redcap_control_center hooks.
 * @see REDCap Hooks documentation
 */
function redcap_control_center() {
    xman_set_menu_item(APP_PATH_WEBROOT . 'ControlCenter/xman.php');
    xman_hook_invoke('redcap_control_center');
}

/**
 * Invokes redcap_custom_verify_username hooks.
 * @see REDCap Hooks documentation
 */
function redcap_custom_verify_username($username) {
    return xman_hook_invoke('redcap_custom_verify_username', null, array($username));
}

/**
 * Invokes redcap_data_entry_form hooks.
 * @see REDCap Hooks documentation
 */
function redcap_data_entry_form($project_id, $record, $instrument, $event_id, $group_id) {
    xman_hook_invoke('redcap_data_entry_form', $project_id, array($project_id, $record, $instrument, $event_id, $group_id));
}

/**
 * Invokes redcap_data_entry_form_top hooks.
 * @see REDCap Hooks documentation
 */
function redcap_data_entry_form_top($project_id, $record, $instrument, $event_id, $group_id) {
    xman_hook_invoke('redcap_data_entry_form_top', $project_id, array($project_id, $record, $instrument, $event_id, $group_id));
}

/**
 * Invokes redcap_every_page_before_render hooks.
 * @see REDCap Hooks documentation
 */
function redcap_every_page_before_render($project_id) {
    // Loading extensions on every page.
    xman_load_extensions($project_id);
    xman_hook_invoke('redcap_every_page_before_render', $project_id, array($project_id));
}

/**
 * Invokes redcap_every_page_top hooks.
 * @see REDCap Hooks documentation
 */
function redcap_every_page_top($project_id) {
    if ($project_id) {
        xman_set_menu_item(APP_PATH_WEBROOT_PARENT . 'xman.php?pid=' . $project_id);
    }
    xman_hook_invoke('redcap_every_page_top', $project_id, array($project_id));
}

/**
 * Invokes redcap_project_home_page hooks.
 * @see REDCap Hooks documentation
 */
function redcap_project_home_page($project_id) {
    xman_hook_invoke('redcap_project_home_page', $project_id, array($project_id));
}

/**
 * Invokes redcap_save_record hooks.
 * @see REDCap Hooks documentation
 */
function redcap_save_record($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id) {
    xman_hook_invoke('redcap_project_home_page', $project_id, array($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id));
}

/**
 * Invokes redcap_survey_complete hooks.
 * @see REDCap Hooks documentation
 */
function redcap_survey_complete($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id) {
    xman_hook_invoke('redcap_survey_complete', $project_id, array($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id));
}

/**
 * Invokes redcap_survey_page hooks.
 * @see REDCap Hooks documentation
 */
function redcap_survey_page($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id) {
    xman_hook_invoke('redcap_survey_page', $project_id, array($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id));
}

/**
 * Invokes redcap_survey_page_top hooks.
 * @see REDCap Hooks documentation
 */
function redcap_survey_page_top($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id) {
    xman_hook_invoke('redcap_survey_page_top', $project_id, array($project_id, $record, $instrument, $event_id, $group_id, $survey_hash, $response_id));
}

/**
 * Invokes redcap_user_rights hooks.
 * @see REDCap Hooks documentation
 */
function redcap_user_rights($project_id) {
    xman_hook_invoke('redcap_user_rights', $project_id, array($project_id));
}

/**
 * Returns filenames matching our file-based, naming convention.
 *
 * @deprecated
 */
function redcap_hooks_find($hook_function, $project_id = '', $logging = FALSE) {
    $found = array_merge(
        glob(APP_PATH_DOCROOT . "../hooks/$hook_function.php"),
        glob(APP_PATH_DOCROOT . "../hooks/$hook_function/*.php"),
        glob(APP_PATH_DOCROOT . "../hooks/pid$project_id/$hook_function.php"),
        glob(APP_PATH_DOCROOT . "../hooks/pid$project_id/$hook_function/*.php")
    );

    if ($logging) {
        $log_file = "/tmp/hook_events.log";
        $desired_keys = ['pid', 'page', 'id', 'auto', 'arm', 'pids', 'redcap_csrf_token', 'type', 'action'];
        $my_request = array();
        foreach ($_REQUEST as $key => $value) {
            if (in_array($key,$desired_keys)) {
                $my_request[$key] = $value;
            }
        }

        file_put_contents($log_file, "$hook_function at " . PAGE . " with " . json_encode($my_request) . "\n", FILE_APPEND);
        if (count($found) > 0 ) {
            file_put_contents($log_file, "found hooks: " . print_r($found, TRUE) . "\n", FILE_APPEND);
        }
    }

    return $found;
}
