import cv2
import filters
import rects
from managers import WindowManager, CaptureManager
from trackers import FaceTracker

class Cameo(object):

    def __init__(self):
        self._window_manager = WindowManager('Cameo', self.on_key_press)
        self._capture_manager = CaptureManager(cv2.VideoCapture(0), self._window_manager, True)
        self._face_tracker = FaceTracker()
        self._should_draw_debug_rects = False
        self._curve_filter = filters.EmbossFilter()  # can use any of applied filters

    def run(self):
        self._window_manager.create_window()
        while self._window_manager.is_window_created:

            self._capture_manager.enter_frame()
            frame = self._capture_manager.frame

            self._face_tracker.update(frame)
            faces = self._face_tracker.faces
            rects.swap_rects(frame, frame, [face.face_rect for face in faces])

            filters.stroke_edges(frame, frame)
            self._curve_filter.apply(frame, frame)

            if self._should_draw_debug_rects:
                self._face_tracker.draw_debug_rects(frame)

            self._capture_manager.exit_frame()
            self._window_manager.process_events()

    def on_key_press(self, keycode):

        """
        Keypress handler

        space - screen
        tab - start/stop recording
        x - start/stop debug
        escape - quit
        :return:
        """

        if keycode == 32:  # space
            self._capture_manager.write_image('screenshot.png')
        elif keycode == 9:  # tab
            if not self._capture_manager.is_writing_video:
                self._capture_manager.start_writing_video('screencast.avi')
            else:
                self._capture_manager.stop_writing_video()
        elif keycode == 120:  # x
            self._should_draw_debug_rects = not self._should_draw_debug_rects
        elif keycode == 27:  # escape
            self._window_manager.destroy_window()


if __name__=="__main__":
    Cameo().run()
