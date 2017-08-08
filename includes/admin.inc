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
        $class = 'darkgreen';
        $icon = 'tick';

        if ($_POST['form_type'] == 'save') {
            $to_del = array();
            foreach ($extensions as $name => $extension) {
                if (!empty($_POST[$name])) {
                    if (!$extension['enabled']) {
                        $extensions[$name]['enabled'] = 1;
                        xman_enable_extension($name);
                    }
                }
                elseif ($extension['enabled']) {
                    $extensions[$name]['enabled'] = 0;
                    $to_del[] = $name;
                }
            }

            if (!empty($to_del)) {
                xman_disable_extensions($to_del);
            }

            $msg = 'Your configuration has been saved successfully.';
        }
        elseif ($_POST['form_type'] == 'update') {
            $msg = 'The updates have been applied successfully.';
            foreach (xman_get_available_updates() as $name => $updates) {
                if (!empty($_POST[$name]) && !xman_update_extension($name, $updates)) {
                    $msg = 'An error has ocurred. Please try again or contact site administration.';
                    $class = 'red';
                    $icon = 'cross';

                    break;
                }
            }
        }

        print '<div class="' . $class . '"><img src="' . APP_PATH_IMAGES . $icon . '.png"> ' . $msg . '</div>';
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
                    <th>Updates</th>
                </tr>
                <?php foreach ($updates as $name => $updates): ?>
                    <tr>
                        <?php $list = array(); ?>
                        <?php foreach ($updates as $version => $description): ?>
                            <?php $list[] = '#' . $version . ': ' . ($description ? $description : 'No description available.'); ?>
                        <?php endforeach; ?>
                        <td>
                            <input type="checkbox" id="<?php print $name; ?>" name="<?php print $name; ?>" value="checked">
                            <label for="<?php print $name; ?>"><?php print $extensions[$name]['name']; ?></label>
                        </td>
                        <td><?php print implode('<br>', $list); ?>
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
