@file:Suppress("UnstableApiUsage")

import org.gradle.api.tasks.testing.logging.TestLogEvent
import java.io.ByteArrayOutputStream

val pkg: String = providers.gradleProperty("wireguardPackageName").get()

plugins {
    alias(libs.plugins.android.library)
    `maven-publish`
    signing
}

val wireguardVersionName = providers.gradleProperty("wireguardVersionName").get()
val wireguardVersionCode = providers.gradleProperty("wireguardVersionCode").get().toInt()

val exec = { commandLine: String ->
    val output = ByteArrayOutputStream()
    output.use { outputStream ->
        exec {
            standardOutput = outputStream
            commandLine("bash", "-c", commandLine)
        }
    }
    String(output.toByteArray()).trim()
}

val countProtonCommits = {
    exec("git log --first-parent $wireguardVersionName..HEAD --oneline | wc -l").toInt()
}

val getProtonVersionName = {
    "$wireguardVersionName.${countProtonCommits()}"
}

extra["protonWireguardVersionCode"] = wireguardVersionCode * 1000 + countProtonCommits()
extra["protonWireguardVersionName"] = getProtonVersionName()

android {
    compileSdk = 33
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    namespace = "${pkg}.tunnel"
    defaultConfig {
        minSdk = 21
    }
    externalNativeBuild {
        cmake {
            path("tools/CMakeLists.txt")
        }
    }
    testOptions.unitTests.all {
        it.testLogging { events(TestLogEvent.PASSED, TestLogEvent.SKIPPED, TestLogEvent.FAILED) }
    }
    buildTypes {
        all {
            externalNativeBuild {
                cmake {
                    targets("libwg-go.so", "libwg.so", "libwg-quick.so")
                    arguments("-DGRADLE_USER_HOME=${project.gradle.gradleUserHomeDir}")
                }
            }
        }
        release {
            externalNativeBuild {
                cmake {
                    arguments("-DANDROID_PACKAGE_NAME=${pkg}")
                }
            }
        }
        debug {
            externalNativeBuild {
                cmake {
                    arguments("-DANDROID_PACKAGE_NAME=${pkg}.debug")
                }
            }
        }
    }
    lint {
        disable += "LongLogTag"
        disable += "NewApi"
    }
    publishing {
        singleVariant("release") {
            withJavadocJar()
            withSourcesJar()
        }
    }
}

dependencies {
    implementation(libs.androidx.annotation)
    implementation(libs.androidx.collection)
    compileOnly(libs.jsr305)
    testImplementation(libs.junit)
}

publishing {
    val githubRepo = "github.com/ProtonVPN/wireguard-android"
    publications {
        register<MavenPublication>("release") {
            groupId = "me.proton.vpn"
            artifactId = "wireguard-android"
            version = extra["protonWireguardVersionName"] as String
            afterEvaluate {
                from(components["release"])
            }
            pom {
                name.set("WireGuard Tunnel Library")
                description.set("Embeddable tunnel library for WireGuard for Android")
                url.set("https://www.wireguard.com/")

                licenses {
                    license {
                        name.set("The Apache Software License, Version 2.0")
                        url.set("http://www.apache.org/licenses/LICENSE-2.0.txt")
                        distribution.set("repo")
                    }
                }
                scm {
                    connection.set("scm:git:git://${githubRepo}.git")
                    developerConnection.set("scm:git:ssh://${githubRepo}.git")
                    url.set("https://${githubRepo}")
                }
                developers {
                    organization {
                        name.set("WireGuard")
                        url.set("https://www.wireguard.com/")
                    }
                    developer {
                        name.set("WireGuard")
                        email.set("team@wireguard.com")
                    }
                    developer {
                        id.set("opensource@proton.me")
                        name.set("Open Source Proton")
                        email.set("opensource@proton.me")
                    }
                }
            }
        }
    }
    repositories {
        maven {
            name = "sonatype"
            url = uri("https://s01.oss.sonatype.org/service/local/staging/deploy/maven2/")
            credentials {
                username = providers.environmentVariable("SONATYPE_USER").orNull
                password = providers.environmentVariable("SONATYPE_PASSWORD").orNull
            }
        }
    }
}

signing {
    val signingKey = findProperty("signingKey").toString()
    val signingPassword = findProperty("signingPassword").toString()
    useInMemoryPgpKeys(signingKey, signingPassword)
    sign(publishing.publications)
}
