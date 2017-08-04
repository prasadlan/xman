<?php
/**
 * @file
 * Basic functions for XMan.
 */

define('APP_PATH_EXTENSIONS', APP_PATH_DOCROOT . '../xman/extensions/');
include_once 'admin.inc';

/**
 * Gets all available XMan plugins().
 */
function xman_get_pages() {
    global $redcap_version;
    $base_path = 'redcap_v' . $redcap_version . '/';

    $pages = xman_hook_invoke('xman_plugins', null, array(), true);
    $pages[$base_path . 'ControlCenter/xman.php'] = 'xman_control_center_page';

    return $pages;
}

/**
 * Adds new item to the control center menu.
 */
function xman_set_control_center_item() {
    print '<div id ="xman-menu-item-wrapper" style="display:none"><img src="' . APP_PATH_IMAGES . 'brick.png">&nbsp; <a href="' . APP_PATH_WEBROOT . 'ControlCenter/xman.php"> Extensions Manager (XMan)</a><br/></div>';
    print '<script src="' . APP_PATH_WEBROOT_PARENT . 'xman/js/control-center.js' . '"></script>';
}

/**
 * Loads all enabled extensions.
 */
function xman_load_extensions($project_id = null) {
    foreach (xman_get_enabled_extensions($project_id) as $name => $extension) {
        $path = APP_PATH_EXTENSIONS . $name . '/' . $name . '.extension';
        if (file_exists($path)) {
            require_once $path;
        }
    }
}

/**
 * Invokes hook execution over all enabled extensions.
 *
 * @param string $hook_name
 *   The hook name (e.g. "redcap_data_entry_form", "redcap_every_page_top").
 * @param int $project_id
 *   The REDCap project ID, if available.
 * @param array $params
 *   The hook execution parameters.
 * @param bool $merge_results
 *   If TRUE, instead of getting
 *
 * @return mixed
 *   If $merge_results is TRUE, it returns all results merged into one single
 *   array.
 *
 *   Otherwise, it returns the last hook execution return value (so the most
 *   "heavier" extension gets the preference).
 *
 */
function xman_hook_invoke($hook_name, $project_id = null, $params = array(), $merge_results = false) {
    $results = array();

    if ($extensions = xman_get_enabled_extensions()) {
        foreach ($extensions as $name => $extension) {
            if ($project_id && !empty($extension['projects'] && !in_array($project_id, $extension['projects']))) {
                continue;
            }

            $function = $name . '_' . $hook_name;
            if (function_exists($function)) {
                $results[] = call_user_func_array($function, $params);
            }
        }
    }

    if (function_exists('redcap_hooks_find')) {
        // TODO: Deprecated - remove this when no dependencies are left.
        if ($hook_files = redcap_hooks_find($hook_name, $project_id)) {
            foreach ($hook_files as $filename) {
                $function = include $filename;
                $result = call_user_func_array($function, $params);
            }
        }
    }

    if (!$merge_results) {
        return end($results);
    }

    $return = array();
    while ($result = array_shift($results)) {
        $return = array_merge_recursive($return, $result);
    }

    return $return;
}

/**
 * Gets enabled extensions, ordered by execution priority.
 *
 * @return array
 *   A list of extensions, keyed by machine name, each of them containing:
 *     - name: The extension human name
 *     - enabled: Flag that shows whether the extension is enabled
 *     - weight: The execution priority level ("lighter" means more priority)
 */
function xman_get_enabled_extensions() {
    $extensions = array();

    foreach (xman_get_extensions() as $name => $extension) {
        if ($extension['enabled']) {
            $extensions[$name] = $extension;
        }
    }

    uasort($extensions, '_redcap_extensions_cmp');
    return $extensions;
}

/**
 * Gets available extensions.
 *
 * @return array
 *   A list of extensions, keyed by machine name, each of them containing:
 *     - name: The extension human name
 *     - enabled: Flag that shows whether the extension is enabled
 *     - weight: The execution priority level ("lighter" means more priority)
 */
function xman_get_extensions() {
    $enabled_extensions = array();

    $q = db_query('SELECT * FROM redcap_extensions ORDER BY weight');
    if (db_num_rows($q)) {
        while ($extension = db_fetch_assoc($q)) {
            $extension['projects'] = json_decode($extension['projects'], true);
            $extension['projects'] = array_combine($extension['projects'], $extension['projects']);
            $enabled_extensions[$extension['name']] = $extension;
            unset($enabled_extensions[$extension['name']]['name']);
        }
    }

    $extensions = array();
    foreach (scandir(APP_PATH_EXTENSIONS) as $name) {
        if (strpos($name, '.') === 0 | !is_dir(APP_PATH_EXTENSIONS . $name)) {
            continue;
        }

        $path = APP_PATH_EXTENSIONS . $name . '/' . $name . '.info';
        if (!file_exists($path)) {
            continue;
        }

        if (!$info = json_decode(file_get_contents($path), true)) {
            continue;
        }

        if (isset($enabled_extensions[$name])) {
            $extensions[$name] = $enabled_extensions[$name] + $info;
            $extensions[$name]['enabled'] = 1;
        }
        else {
            $extensions[$name] = $info;
        }
    }

    return $extensions;
}

/**
 * Comparison function that defines priority between two extensions.
 *
 * @param array $a
 *   The extension to be compared. Expects a 'weight' key.
 * @param array $b
 *   The extension to compare with. Expects a 'weight' key.
 *
 * @return int
 *   0 if $a and $b have same weight, -1 if $a is lighter, 1 if $a is heavier.
 */
function _xman_extensions_cmp($a, $b) {
    if ($a['weight'] == $b['weight']) {
        return 0;
    }

    return ($a['weight'] < $b['weight']) ? -1 : 1;
}
