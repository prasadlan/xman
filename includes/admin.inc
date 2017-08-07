<?php
/**
 * @file
 * XMan administrative pages callbacks.
 */

/**
 * Displays XMan Control Center page.
 */
function xman_control_center_page() {
    global $lang;
    include APP_PATH_DOCROOT . 'ControlCenter/header.php';

    $extensions = xman_get_extensions();

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if ($_POST['form_type'] == 'save') {
            $to_del = array();
            foreach ($extensions as $name => $extension) {
                if (!empty($_POST[$name])) {
                    if (!$extension['enabled']) {
                        $extensions[$name]['enabled'] = 1;
                        xman_hook_invoke('xman_extension_enable', null, array($extensions[$name]));
                        db_query('INSERT INTO redcap_extensions (name, weight, schema_version, projects) VALUES ("' . db_escape($name) . '", 0, 0, "[]")');

                        $updates = xman_get_available_updates();
                        if (isset($updates[$name])) {
                            db_query('UPDATE redcap_extensions SET schema_version = ' . end($updates[$name]) . ' WHERE name = "' . db_escape($name) . '"');
                        }
                    }
                }
                elseif ($extension['enabled']) {
                    $extensions[$name]['enabled'] = 0;
                    $to_del[] = $name;
                    xman_hook_invoke('xman_extension_disable', null, array($extensions[$name]));
                }
            }

            if (!empty($to_del)) {
                db_query('DELETE FROM redcap_extensions WHERE name IN ("' . implode('", "', $to_del) . '")');
            }

            $msg = 'Your configuration has been saved successfully.';
        }
        elseif ($_POST['form_type'] == 'update') {
            foreach (xman_get_available_updates() as $name => $updates) {
                if (!empty($_POST[$name])) {
                    xman_update_extension($name, $updates);
                }
            }

            $msg = 'The updates have been applied successfully.';
        }

        print '<div class="darkgreen"><img src="' . APP_PATH_IMAGES . 'tick.png"> ' . $msg . '</div>';
    }
?>

<h4><img src="<?php print APP_PATH_IMAGES; ?>brick.png"> Extensions</h4>
<?php if ($extensions): ?>
    <form action="<?php print APP_PATH_WEBROOT; ?>ControlCenter/xman.php" enctype="multipart/form-data" target="_self" method="post" name="xman-save-form" id="xman-save-form">
        <input type="hidden" name="form_type" value="save">
        <table class="table">
            <tr>
                <th>Name</th>
                <th>Description</th>
                <th>Type</th>
                <th>Version</th>
                <th>Authors</th>
            </tr>
            <?php foreach ($extensions as $name => $info): ?>
                <tr>
                    <td style="min-width:120px;">
                        <input type="checkbox" id="<?php print $name; ?>" name="<?php print $name; ?>" value="enabled"<?php print $info['enabled'] ? ' checked' : ''; ?>>
                        <label for="<?php print $name; ?>"><?php print $info['name']; ?></label>
                    </td>
                    <td style="min-width:120px;"><?php print empty($info['description']) ? '-' : htmlspecialchars($info['description']); ?></td>
                    <td><?php print empty($info['global']) ? 'Project' : 'Global'; ?></td>
                    <td><?php print empty($info['version']) ? '-' : $info['version']; ?></td>
                    <td><?php print empty($info['authors']) ? '-' : htmlspecialchars($info['authors']); ?></td>
                </tr>
            <?php endforeach; ?>
        </table>
        <div class="text-center">
            <button class="btn btn-success" type="submit">Save</button>
        </div>
    </form>

    <?php if ($updates = xman_get_available_updates()): ?>
        <h4><img src="<?php print APP_PATH_IMAGES; ?>arrow_refresh.png"> Available extension updates</h4>
        <form action="<?php print APP_PATH_WEBROOT; ?>ControlCenter/xman.php" enctype="multipart/form-data" target="_self" method="post" name="xman-update-form" id="xman-update-form">
            <input type="hidden" name="form_type" value="update">
            <table class="table">
                <tr>
                    <th>Extension</th>
                    <th>Update IDs</th>
                </tr>
                <?php foreach ($updates as $name => $updates): ?>
                    <tr>
                        <td>
                            <input type="checkbox" id="<?php print $name; ?>" name="<?php print $name; ?>" value="checked">
                            <label for="<?php print $name; ?>"><?php print $extensions[$name]['name']; ?></label>
                        </td>
                        <td><?php print implode(', ', $updates); ?>
                    </tr>
                <?php endforeach; ?>
            </table>
            <div class="text-center">
                <button class="btn btn-primary" type="submit">Run updates</button>
            </div>
        </form>
    <?php endif; ?>
<?php else: ?>
    <div class="text-info">No extensions available.</div>
<?php endif; ?>

<?php
    include APP_PATH_DOCROOT . 'ControlCenter/footer.php';
}
