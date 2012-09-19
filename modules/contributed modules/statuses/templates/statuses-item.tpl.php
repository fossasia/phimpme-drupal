<?php

/**
 * @file
 *   Displays individual status updates.
 *
 * See http://drupal.org/node/226776 for a list of default variables.
 *
 * Other variables available:
 * - $sid: The status message ID
 * - $meta: Information about the context of the status message, like "to [recipient]"
 * - $self: Whether the status is an update to the sender's own status
 * - $page: Whether the status is being displayed on its own page
 * - $in_context: Whether the recipient is obvious from the context of the page
 * - $type: The recipient type
 * - $recipient: The recipient object
 * - $recipient_name: The (safe) recipient name
 * - $recipient_link: A link to the recipient
 * - $recipient_picture: The recipient's picture, if applicable
 * - $sender: The sender object
 * - $sender_name: The (safe) sender name
 * - $sender_link: A themed link to the sender
 * - $sender_picture: The sender's picture
 * - $created: The themed message created time
 * - $message: The themed status message
 * - $links: Status links (edit/delete/respond/share)
 * - $status_url: The URL of the status message
 * - $status: The status object
 * - $context: The context array
 *
 * If the Statuses Comments module is enabled, these variables
 * are also available:
 * - $comments: Comments on the relevant status plus the form to leave a comment
 *
 * If the Statuses Private Statuses module is enabled, these
 * variables are also available:
 * - $private: Whether the status update is private or not
 * - $private_text: The translated version of either "Private" or "Public"
 *
 * If the (third-party) Facebook-style Micropublisher module is enabled, these
 * variables are also available:
 * - $attachment: The themed attachment to the status update
 *
 * Other modules may add additional variables.
 */
?>
<div id="statuses-item-<?php echo $sid; ?>" class="statuses-item statuses-media statuses-type-<?php echo $type; ?><?php if ($self): ?> statuses-self-update<?php endif; ?><?php if ($page): ?> statuses-page<?php endif; ?><?php if (!empty($private)): ?> statuses-private<?php endif; ?>">
  <?php if (!empty($sender_picture)) : ?>
    <div class="statuses-sender-picture user-picture"><?php echo $sender_picture; ?></div>
  <?php endif; ?>
  <div class="content">
    <?php if (!empty($sender_link) && !empty($recipient_link)): ?>
      <div class="statuses-participants">
      <?php if (!empty($sender_link)) : ?>
        <span class="statuses-sender"><?php echo $sender_link; ?></span>
      <?php endif; ?>
      <?php if (!empty($recipient_link) && !$in_context): ?>
        &raquo; <span class="statuses-recipient"><?php echo $recipient_link; ?></span>
      <?php endif; ?>
      <?php if (!empty($private)) : ?>
        <span class="statuses-private-text"><?php echo $private_text; ?></span>
      <?php endif; ?>
      </div>
    <?php endif; ?>
    <div class="statuses-content"><?php echo $message; ?></div>
    <?php if (!empty($attachment)) : ?>
      <div class="fbsmp clearfix"><?php echo $attachment; ?></div>
    <?php endif; ?>
    <?php if (!empty($created) || !empty($meta) || !empty($links)) : ?>
      <div class="statuses-details">
        <?php if (!empty($links)) : ?>
          <div class="statuses-links"><?php echo $links; ?></div>
        <?php endif; ?>
        <?php if (!empty($created)) : ?>
          <div class="statuses-time">
            <?php if (!$page): ?>
              <a href="<?php echo $status_url; ?>">
            <?php endif; ?>
            <?php echo $created; ?>
            <?php if (!$page): ?>
              </a>
            <?php endif; ?>
          </div>
        <?php endif; ?>
      </div>
    <?php endif; ?>
    <?php if (!empty($comments)) : ?>
      <div class="statuses-comments"><?php echo $comments; ?></div>
    <?php endif; ?>
  </div>
</div>
