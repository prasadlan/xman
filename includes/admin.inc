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
        $to_del = array();
        foreach ($extensions as $name => $extension) {
            if (!empty($_POST[$name])) {
                if (!$extension['enabled']) {
                    // TODO: invoke hook_xman_enable().
                    db_query('INSERT INTO redcap_extensions (name, weight, projects) VALUES ("' . db_escape($name) . '", 0, "[]")');
                    $extensions[$name]['enabled'] = 1;
                }
            }
            elseif ($extension['enabled']) {
                // TODO: invoke hook_xman_disable().
                $extensions[$name]['enabled'] = 0;
                $to_del[] = $name;
            }
        }

        if (!empty($to_del)) {
            db_query('DELETE FROM redcap_extensions WHERE name IN ("' . implode('", "', $to_del) . '")'); 
        }

        print '<div class="darkgreen"><img src="' . APP_PATH_IMAGES . 'tick.png"> Your configuration has been saved successfully.</div>';
    }
?>

<h4><img src="<?php print APP_PATH_IMAGES; ?>brick.png"> Extensions</h4>
<form action="<?php print APP_PATH_WEBROOT; ?>ControlCenter/xman.php" enctype="multipart/form-data" target="_self" method="post" name="form" id="form">
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

<?php
    include APP_PATH_DOCROOT . 'ControlCenter/footer.php';
}
