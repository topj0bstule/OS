package com.example.todoapi.service;

import com.example.todoapi.model.Task;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.test.util.ReflectionTestUtils;

import javax.xml.validation.Schema;
import javax.xml.validation.Validator;
import javax.xml.validation.ValidatorHandler;
import java.util.Arrays;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
public class XmlValidationServiceTest {
    
    @Mock
    private Schema taskSchema;
    
    @Mock
    private Validator validator;
    
    private XmlValidationService xmlValidationService;
    
    @BeforeEach
    void setUp() throws Exception {
        xmlValidationService = new XmlValidationService();
        // Используем ReflectionTestUtils вместо рефлексии
        ReflectionTestUtils.setField(xmlValidationService, "taskSchema", taskSchema);
    }
    
    @Test
    void validateXml_ValidXml_ShouldReturnTrue() throws Exception {
        // Arrange
        String validXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <tasks>
                <task>
                    <title>Test Task</title>
                    <status>TODO</status>
                </task>
            </tasks>
            """;
        
        // Настраиваем мок ТОЛЬКО для этого теста
        when(taskSchema.newValidator()).thenReturn(validator);
        
        // Act
        boolean result = xmlValidationService.validateXml(validXml);
        
        // Assert
        assertTrue(result);
        verify(validator, times(1)).setProperty(eq("http://javax.xml.XMLConstants/property/accessExternalDTD"), eq(""));
        verify(validator, times(1)).setProperty(eq("http://javax.xml.XMLConstants/property/accessExternalSchema"), eq(""));
        verify(validator, times(1)).validate(any());
    }
    
    @Test
    void validateXml_EmptyXml_ShouldReturnFalse() {
        // Arrange
        String emptyXml = "";
        
        // Act
        boolean result = xmlValidationService.validateXml(emptyXml);
        
        // Assert
        assertFalse(result);
        // НЕ вызываем when() для taskSchema.newValidator() - мок не используется
    }
    
    @Test
    void validateXml_NullXml_ShouldReturnFalse() {
        // Arrange
        String nullXml = null;
        
        // Act
        boolean result = xmlValidationService.validateXml(nullXml);
        
        // Assert
        assertFalse(result);
        // НЕ вызываем when() для taskSchema.newValidator() - мок не используется
    }
    
    @Test
    void validateXml_InvalidXml_ShouldReturnFalse() throws Exception {
        // Arrange
        String invalidXml = "invalid xml content";
        
        // Настраиваем мок для этого теста
        when(taskSchema.newValidator()).thenReturn(validator);
        
        // Бросаем RuntimeException, так как ValidatorException требует SAXException
        doThrow(new org.xml.sax.SAXException("Invalid XML"))
        .when(validator).validate(any());
        
        // Act
        boolean result = xmlValidationService.validateXml(invalidXml);
        
        // Assert
        assertFalse(result);
        verify(validator, times(1)).validate(any());
    }
    
    @Test
    void tasksToXml_ShouldThrowException() throws Exception {
        // Arrange
        Task task = new Task("Test", "Desc", Task.TaskStatus.TODO);
        List<Task> tasks = Arrays.asList(task);
        
        // Act & Assert
        assertThrows(Exception.class, () -> {
            xmlValidationService.tasksToXml(tasks);
        });
    }
    
    @Test
    void taskToXml_ShouldThrowException() throws Exception {
        // Arrange
        Task task = new Task("Test", "Desc", Task.TaskStatus.TODO);
        
        // Act & Assert
        assertThrows(Exception.class, () -> {
            xmlValidationService.taskToXml(task);
        });
    }
}