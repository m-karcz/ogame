from robot.api.deco import keyword
from selenium import webdriver
from selenium.webdriver import FirefoxOptions

from webdriver_manager.firefox import GeckoDriverManager
import time

BASE_URL = "http://localhost:8888/"

class OgameLib2:
    ROBOT_LIBRARY_SCOPE = 'TEST'
    ROBOT_AUTO_KEYWORDS = False

    def __init__(self):
        opts = FirefoxOptions()
        opts.add_argument("--headless")
        self.driver = webdriver.Firefox(executable_path=GeckoDriverManager().install(), firefox_options=opts)

    @keyword
    def open_site(self):
        self.driver.get(BASE_URL)


    @keyword
    def register_and_login_on(self, login="elo", passcode="320"):
        self.driver.find_element_by_name("login-input").send_keys(login)
        self.driver.find_element_by_name("passcode-input").send_keys(passcode)
        self.driver.find_element_by_xpath('//input[@value="Register"]').click()
        time.sleep(1)
        self.driver.find_element_by_xpath('//input[@value="Login"]').click()

    @keyword
    def assert_logged_in(self):
        if any(self.driver.find_elements_by_name("login-input")):
            raise RuntimeError("not logged in")
         
